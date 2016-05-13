#ifndef REGEX_NFAENGINE_H
#define REGEX_NFAENGINE_H

#include "Parser.hpp"
#include "NFA.hpp"
#include <unordered_map>
#include <vector>
#include <queue>
#include <unordered_set>

namespace RE
{
    using std::unordered_map;
    using std::vector;

    class NFAEngine
    {
    public:
        NFAEngine(const string& str)
            : nfaCons(new NFAConstructor),
              nfa( nfaCons->construct( parse(str) ) )
        {}

        string search(const string& str)
        {
        }

        bool match(const string& str)
        {
            s = str;

            unordered_map< Node*, vector<int> > map1;
            unordered_map< Node*, vector<int> > map2;

            unordered_map< Node*, vector<int> >& oldmap = map1;
            unordered_map< Node*, vector<int> >& newmap = map2;

            vector<int> grp = vector<int>(2 * nfa->maxGroup);
            oldmap[nfa->start] = grp;
            addEpsilon(oldmap, nfa->start, grp);

            int i = 0;
            while(i < str.size() && !oldmap.empty())
            {
                for(auto& status : oldmap)
                {
                    Node* st = status.first;
                    vector<int>& grp = status.second;

                    if(st->group != -1)
                        grp[st->group] = i;

                    auto iter = st->edges.find('.');
                    if(iter != st->edges.end())
                    {
                        for(Node* each : iter->second)
                        {
                            newmap[each] = grp;
                            addEpsilon(newmap, each, grp);
                        }
                    }

                    iter = st->edges.find(str[i]);
                    if(iter != st->edges.end())
                    {
                        for(Node* each : iter->second)
                        {
                            newmap[each] = grp;
                            addEpsilon(newmap, each, grp);
                        }
                    }
                }

                i++;
                oldmap.clear();
                std::swap(oldmap, newmap);
            }

            if(i < str.size()) return false;

            auto iter = oldmap.find(nfa->end);
            if(iter != oldmap.end())
            {
                group = iter->second;
                return true;
            }

            return false;
        }

    private:
        Parser parse;
        NFAConstructorPtr nfaCons;
        NFAPtr nfa;
        string s;
        vector<int> group;

        using StrSizeType = string::size_type;

        void addEpsilon(unordered_map< Node*, vector<int> >& newmap,
                        Node* start, vector<int>& grp)
        {
            std::queue<Node*> Q;
            std::unordered_set<Node*> isVisited;

            Q.push(start);
            isVisited.insert(start);

            while( !Q.empty() )
            {
                Node* n = Q.front();
                Q.pop();

                auto iter = n->edges.find(0);
                if(iter == n->edges.end()) continue;
                for(auto& eachNode : iter->second)
                {
                    if(isVisited.find(eachNode) != isVisited.end())
                        continue;

                    newmap[eachNode] = grp;

                    Q.push(eachNode);
                    isVisited.insert(eachNode);
                }

            }
        }

    }; // class NFAEngine

} // namespace RE

#endif // REGEX_NFAENGINE_H