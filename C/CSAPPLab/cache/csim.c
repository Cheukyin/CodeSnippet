#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>

#define true 1
#define false 0

typedef unsigned char bool;
typedef unsigned long long uint64;
typedef long long int64;

typedef struct cache_line_t
{
    uint64 tag;
    struct cache_line_t *prev;
    struct cache_line_t *next;
}cache_line_t;

typedef struct cache_set_t
{
    cache_line_t *lines_head_in_use;
    cache_line_t *lines_tail_in_use;

    cache_line_t *lines_head_free;
    cache_line_t *lines_tail_free;
}cache_set_t;

typedef struct cache_args_t
{
    int bits_sets;
    int total_lines_per_set;
    int bits_blocks_per_line;
    bool verbose_flag;
    FILE *trace_file;
}cache_args_t;

typedef struct cache_trace_result_t
{
    int hit;
    int miss;
    int eviction;
}cache_trace_result_t;

void free_cache_args(cache_args_t *cache_args)
{
    fclose(cache_args->trace_file);
    free(cache_args);
}

//insert new_line after line
static inline void insert_cache_line_after(cache_line_t *new_line, cache_line_t *line)
{
    new_line->next = line->next;
    new_line->prev = line;
    line->next = new_line;
    new_line->next->prev = new_line;
    //for debug
    /* static int k; */
    /* new_line->tag = k++; */
}

//delete the del_line from the linked list, return the del_line
cache_line_t *delete_cache_line(cache_line_t *del_line)
{
    del_line->prev->next = del_line->next;
    del_line->next->prev = del_line->prev;

    del_line->next = del_line->prev = NULL;
    return del_line;
}

//search tag between lines_head and lines_tail
//if not found, return NULL
cache_line_t *search_cache_line_by_tag(uint64 tag, cache_line_t *lines_head, cache_line_t *lines_tail)
{
    cache_line_t *cur_line;
    for(cur_line = lines_head->next; cur_line != lines_tail; cur_line = cur_line->next)
        if(cur_line->tag == tag)
            return cur_line;

    return NULL;
}

bool is_cache_lines_empty(cache_line_t *lines_head)
{
    return lines_head->next->next == NULL;
}


cache_set_t *init_cache_set(uint64 total_sets, uint64 total_lines_per_set)
{
    cache_set_t *cache_sets = (cache_set_t*)malloc( sizeof(cache_set_t)*total_sets );
    for(uint64 i=0; i<total_sets; i++)
    {
        #define Init_Lines_Head_Tail(freeOruse) \
                do{ \
                    cache_sets[i].lines_head_##freeOruse = (cache_line_t*)malloc( sizeof(cache_line_t) ); \
                    cache_sets[i].lines_tail_##freeOruse = (cache_line_t*)malloc( sizeof(cache_line_t) ); \
                    cache_sets[i].lines_head_##freeOruse->prev = NULL; \
                    cache_sets[i].lines_head_##freeOruse->next = cache_sets[i].lines_tail_##freeOruse; \
                    cache_sets[i].lines_tail_##freeOruse->prev = cache_sets[i].lines_head_##freeOruse; \
                    cache_sets[i].lines_tail_##freeOruse->next = NULL; \
                }while(0)

        Init_Lines_Head_Tail(free);
        Init_Lines_Head_Tail(in_use);

        //malloc free list for each set
        for(uint64 k=0; k<total_lines_per_set; k++)
        {
            cache_line_t *new_free_line = (cache_line_t*)malloc( sizeof(cache_line_t) );
            insert_cache_line_after(new_free_line, cache_sets[i].lines_head_free);
        }
    }

    return cache_sets;
}

void free_cache_lines(cache_line_t *cache_lines_head)
{
    cache_line_t *cur_line = cache_lines_head;
    while(cur_line)
    {
        //for debug
        /* printf("\tcache line: %d\n", cur_line->tag); */
        cache_line_t *tmp = cur_line->next;
        free(cur_line);
        cur_line = tmp;
    }
}

void free_cache_set(cache_set_t *cache_sets, uint64 total_sets)
{
    /* printf("total sets: %d\n", total_sets); */
    for(uint64 i=0; i<total_sets; i++)
    {
        //for debug
        /* printf("cache set: %d\n", i); */
        /* printf("cache line free: \n"); */
        free_cache_lines( cache_sets[i].lines_head_free );
        /* printf("cache line in use: \n"); */
        free_cache_lines( cache_sets[i].lines_head_in_use );
    }

    free(cache_sets);
}

cache_trace_result_t *cache_trace(cache_set_t *cache_sets, cache_args_t *cache_args)
{
    cache_trace_result_t *cache_trace_result = (cache_trace_result_t*)malloc( sizeof(cache_trace_result_t) );
    cache_trace_result->hit = cache_trace_result->miss = cache_trace_result->eviction = 0;

    /* uint64 cache_block_size = ((uint64)1) << (cache_args->bits_blocks_per_line); */
    int bits_tag_each_line = 64 - cache_args->bits_sets - cache_args->bits_blocks_per_line;

    //addr is 64-bit
    #define extract_set_bits(addr) \
        ( (uint64)( (uint64)(addr) & ~((int64)1<<63>>(bits_tag_each_line-1)) ) >> (cache_args->bits_blocks_per_line) )
    //test the macro extract_set_bits
    #if 0
        cache_args->bits_blocks_per_line = 8;
        bits_tag_each_line = 16;
        assert( extract_set_bits(0xf1f2f34455667788ul) == 0xf344556677ul );
    #endif

    //addr is 64-bit
    #define extract_tag_bits(addr) \
        ( (uint64)(addr) >> (64 - bits_tag_each_line) )
    //test the macro extract_bit_bits
    #if 0
        bits_tag_each_line = 48;
        assert( extract_tag_bits(0xff22334455667788ul) == 0xff2233445566ul );
    #endif


    #define MAXLINE 1024
    char trace_line[MAXLINE];
    while( fgets(trace_line, MAXLINE, cache_args->trace_file) )
    {
        //op is 'L' , 'S' or 'M'
        char op;
        uint64 addr;
        uint64 op_size; //bytes
        sscanf(trace_line, " %c %llx,%llx", &op, &addr, &op_size);
        if( op == 'I' )	continue;

        cache_set_t cur_set = cache_sets[ extract_set_bits(addr) ];
        uint64 tag = extract_tag_bits(addr);
        cache_line_t *line_found = search_cache_line_by_tag(tag, cur_set.lines_head_in_use, cur_set.lines_tail_in_use);


        #define print_verbose(...) \
                do{ \
                    if( cache_args->verbose_flag ) \
                        printf( __VA_ARGS__ ); \
                  }while(0)


        print_verbose("%c %llx,%llx ", op, addr, op_size );
        //hit
        if(line_found)
        {
            cache_trace_result->hit++;
            print_verbose("hit");

            delete_cache_line(line_found);
            insert_cache_line_after(line_found, cur_set.lines_head_in_use);
        }
        //miss
        else
        {
            cache_trace_result->miss++;
            print_verbose("miss ");

            if( !is_cache_lines_empty( cur_set.lines_head_free ) )
            {
                //find a line from the free lines
                line_found = delete_cache_line( cur_set.lines_head_free->next );
                line_found->tag = tag;
                insert_cache_line_after(line_found, cur_set.lines_head_in_use);
            }
            //eviction
            else
            {
                cache_trace_result->eviction++;
                print_verbose("eviction ");

                //LRU
                line_found = delete_cache_line( cur_set.lines_tail_in_use->prev );
                line_found->tag = tag;
                insert_cache_line_after(line_found, cur_set.lines_head_in_use);
            }
        }

        if( op == 'M' )
        {
            cache_trace_result->hit++;
            print_verbose("hit");
        }
        print_verbose("\n");
    }

    return cache_trace_result;
}

cache_args_t *cmd_parser(int argc, char *argv[])
{
    char opt;
    opterr = 0;

    cache_args_t *cache_args = (cache_args_t*)malloc( sizeof(cache_args_t) );
    cache_args->bits_sets = 0;
    cache_args->total_lines_per_set = 0;
    cache_args->bits_blocks_per_line = 0;
    cache_args->verbose_flag = false;
    cache_args->trace_file = NULL;

    while( (opt = getopt(argc, argv, "s:E:b:t:h::v::")) != -1)
    {
        switch( opt )
        {
            case 's':
                cache_args->bits_sets = atoi(optarg);
                opterr = cache_args->bits_sets? 0 : 1;
                break;
            case 'E':
                cache_args->total_lines_per_set = atoi(optarg);
                opterr = cache_args->total_lines_per_set? 0 : 2;
                break;
            case 'b':
                cache_args->bits_blocks_per_line = atoi(optarg);
                opterr = cache_args->bits_blocks_per_line? 0 : 3;
                break;
            case 't':
                cache_args->trace_file = fopen(optarg, "r");
                opterr = cache_args->trace_file? 0 : 4;
                break;
            case 'v':
                cache_args->verbose_flag = true;
                break;
            default:
                printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
                printf("Options:\n");
                printf("\t-h\tPrint this help message.\n");
                printf("\t-v\tOptional verbose flag.\n");
                printf("\t-s <num>\tNumber of set index bits.\n");
                printf("\t-E <num>\tNumber of lines per set.\n");
                printf("\t-b <num>\tNumber of block offset bits.\n");
                printf("\t-t <file>\tTrace file.\n");
                printf("\n");
                printf("Examples:\n");
                printf("\tlinux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
                printf("\tlinux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
                printf("\n");
                break;
        }

        if(opterr)
        {
            switch(opterr)
            {
                case 1:
                    printf("bits of cache set is not correct.\n");
                    break;
                case 2:
                    printf("bits of cache line is not correct.\n");
                    break;
                case 3:
                    printf("bits of block size is not correct.\n");
                    break;
                case 4:
                    printf("the trace file does not exist.\n");
                    break;
            }
            exit(1);
        }
    }

    return cache_args;
}

int main(int argc, char *argv[])
{
    //parse the cmd options
    cache_args_t *cache_args = cmd_parser(argc, argv);
    //initial cache sets, each set contains the free lines
    cache_set_t *cache_sets = init_cache_set(1 << (cache_args->bits_sets), \
                                             cache_args->total_lines_per_set);

    cache_trace_result_t *cache_trace_result = cache_trace(cache_sets, cache_args);
    printSummary(cache_trace_result->hit, \
                 cache_trace_result->miss, \
                 cache_trace_result->eviction);

    free(cache_trace_result);
    free_cache_set(cache_sets, 1 << (cache_args->bits_sets));
    free_cache_args(cache_args);

    return 0;
}