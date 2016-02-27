#ifndef REGEX_NFA_H
#define REGEX_NFA_H

#include "Visitor.hpp"
#include <memory>
#include <vector>

namespace RE
{
    struct Node;
    struct Edge;
    using NodePtr = Node*;
    using std::vector;

    struct Node
    {
        vector<Edge> edges; // 0 for epsilon
        int group; // -1 for anonymous group

        Node() : group(-1) {}
        Node(int g) : group(g) {}
    };

    struct Edge
    {
        char c;
        NodePtr node;

        Edge(char ch, NodePtr n) : c(ch), node(n) {}
    };

    struct NFA
    {
        NodePtr start;
        NodePtr end;

        vector<NodePtr> nodes;

        ~NFA()
        {
            NodePool* pool = NodePool::getPool();
            for(NodePtr node : nodes)
                pool->releaseObject(node);
        }
    };

    class NFAConstructor : public Visitor, public std::enable_shared_from_this<NFAConstructor>
    {
    private:
        NFA nfa;

    public:
        NFA construct(const RegexPtr& re)
        {
            nfa = new NFA;
            visit(GroupPtr(re, 0));
            return nfa;
        }

        static NFA buildSimpleNFA(char ch)
        {
            NodePtr start(new Node());
            NodePtr end(new Node());
            start->edges.push_back( Edge(ch, end) );

            return NFA(start, end);
        }

        void visit(const RegexPtr& re) override
        { re->accept( shared_from_this() ); }

        void visit(const EmptyPtr& re) override
        { return; }

        void visit(const NullPtr& re) override
        { nfa = buildSimpleNFA(0); }

        void visit(const CharPtr& re) override
        { nfa = buildSimpleNFA(re->ch); }

        void visit(const AltPtr& re) override
        {
            re->left->accept( shared_from_this() );
            NFA lnfa = nfa;
            re->right->accept( shared_from_this() );
            NFA rnfa = nfa;

            nfa = NFA(NodePtr(new Node()), NodePtr(new Node()));

            nfa.start->edges.push_back( Edge(0, lnfa.start) );
            nfa.start->edges.push_back( Edge(0, rnfa.start) );

            lnfa.end->edges.push_back( Edge(0, nfa.end) );
            rnfa.end->edges.push_back( Edge(0, nfa.end) );
        }

        void visit(const SeqPtr& re) override
        {
            re->first->accept( shared_from_this() );
            NFA fnfa = nfa;
            re->last->accept( shared_from_this() );
            fnfa.end->edges.push_back( Edge(0, nfa.start) );

            nfa = NFA(fnfa.start, nfa.end);
        }

        void visit(const RepPtr& re) override
        {
            re->re->accept( shared_from_this() );
            NFA rnfa = nfa;

            nfa = NFA(NodePtr(new Node()), NodePtr(new Node()));

            nfa.start->edges.push_back( Edge(0, rnfa.start) );
            nfa.start->edges.push_back( Edge(0, nfa.end) );

            rnfa.end->edges.push_back( Edge(0, rnfa.start) );
            rnfa.end->edges.push_back( Edge(0, nfa.end) );
        }

        void visit(const GroupPtr& re) override
        {
            re->re->accept( shared_from_this() );
            nfa.start->group = re->group * 2;
            nfa.start->group = re->group * 2 + 1;
        }

    }; // struct NFAConstructor

} // namespace RE

#endif // REGEX_NFA_H