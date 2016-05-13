#ifndef REGEX_NFA_H
#define REGEX_NFA_H

#include "Visitor.hpp"
#include "CachedObject.hpp"
#include <memory>
#include <vector>
#include <queue>
#include <unordered_set>
#include <map>
#include <string>

namespace RE
{
    using std::vector;
    using std::map;

    struct NFA;
    using NFAPtr = std::shared_ptr<NFA>;

    class NFAConstructor;
    using NFAConstructorPtr = std::shared_ptr<NFAConstructor>;

    struct Node: public CachedObject<Node>
    {
        map< char, vector<Node*> > edges; // 0 for epsilon
        int group; // -1 for anonymous group

        Node() : group(-1) {}
        Node(int g) : group(g) {}

        void addEdge(char c, Node* n)
        {
            auto iter = edges.find(c);
            if(iter != edges.end())
                iter->second.push_back(n);
            else edges[c] = vector<Node*>(1, n);
        }
    };

    struct NFA
    {
        Node* start;
        Node* end;

        int maxGroup;

        vector<Node*> nodes;

        ~NFA()
        {
            for(Node* node : nodes)
                delete node;
        }

        // just for unittest
        std::string bfs()
        {
            if(nodes.size() == 0) return "";

            using std::queue;

            std::string res;

            queue<Node*> Q;
            Q.push(start);

            std::unordered_set<Node*> isVisited;
            isVisited.insert(start);

            while( !Q.empty() )
            {
                Node* n = Q.front();
                Q.pop();

                for(auto& edges : n->edges)
                {
                    for(Node* eachNode : edges.second)
                    {
                        if(isVisited.find(eachNode) != isVisited.end())
                            continue;

                        if(edges.first == 0)
                            res.push_back('0');
                        else
                            res.push_back(edges.first);

                        Q.push(eachNode);
                        isVisited.insert(eachNode);
                    }
                }
            }

            return res;
        }
    };

    class NFAConstructor
        : public Visitor,
          public std::enable_shared_from_this<NFAConstructor>
    {
    private:
        NFAPtr nfa;

    public:
        NFAPtr construct(const RegexPtr& re)
        {
            nfa.reset(new NFA);
            visit( GroupPtr(new Group(re, 0)) );
            nfa->maxGroup = re->maxGroup;
            return nfa;
        }

        void buildSimpleNFA(char ch)
        {
            nfa->start = new Node;
            nfa->end = new Node;
            nfa->start->addEdge(ch, nfa->end);

            nfa->nodes.push_back(nfa->start);
            nfa->nodes.push_back(nfa->end);
        }

        void visit(const RegexPtr& re) override
        { re->accept( shared_from_this() ); }

        void visit(const EmptyPtr& re) override
        { return; }

        void visit(const NullPtr& re) override
        {
            nfa->start = new Node;
            nfa->end = nfa->start;
            nfa->nodes.push_back(nfa->start);
        }

        void visit(const CharPtr& re) override
        { buildSimpleNFA(re->ch); }

        void visit(const AltPtr& re) override
        {
            re->left->accept( shared_from_this() );
            Node* lstart = nfa->start;
            Node* lend = nfa->end;
            re->right->accept( shared_from_this() );
            Node* rstart = nfa->start;
            Node* rend = nfa->end;

            nfa->start = new Node;
            nfa->end = new Node;
            nfa->nodes.push_back(nfa->start);
            nfa->nodes.push_back(nfa->end);

            nfa->start->addEdge(0, lstart);
            nfa->start->addEdge(0, rstart);

            lend->addEdge(0, nfa->end);
            rend->addEdge(0, nfa->end);
        }

        void visit(const SeqPtr& re) override
        {
            re->first->accept( shared_from_this() );
            Node* lstart = nfa->start;
            Node* lend = nfa->end;
            re->last->accept( shared_from_this() );
            Node* rstart = nfa->start;
            Node* rend = nfa->end;

            lend->addEdge(0, rstart);

            nfa->start = lstart;
            nfa->end = rend;
        }

        void visit(const RepPtr& re) override
        {
            re->re->accept( shared_from_this() );
            Node* rstart = nfa->start;
            Node* rend = nfa->end;

            nfa->start = new Node;
            nfa->end = nfa->start;
            nfa->nodes.push_back(nfa->start);

            nfa->start->addEdge(0, rstart);
            rend->addEdge(0, nfa->end);
        }

        void visit(const GroupPtr& re) override
        {
            re->re->accept( shared_from_this() );
            nfa->start->group = re->group * 2;
            nfa->end->group = re->group * 2 + 1;
        }

    }; // struct NFAConstructor

} // namespace RE

#endif // REGEX_NFA_H