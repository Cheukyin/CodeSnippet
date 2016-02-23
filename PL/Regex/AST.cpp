#include "AST.hpp"
#include "Visitor.hpp"

namespace RE
{
    void Empty::accept(const VisitorPtr& visitor)
    { visitor->visit( shared_from_this() ); }

    void Null::accept(const VisitorPtr& visitor)
    { visitor->visit( shared_from_this() ); }

    void Char::accept(const VisitorPtr& visitor)
    { visitor->visit( shared_from_this() ); }

    void Alt::accept(const VisitorPtr& visitor)
    { visitor->visit( shared_from_this() ); }

    void Seq::accept(const VisitorPtr& visitor)
    { visitor->visit( shared_from_this() ); }

    void Rep::accept(const VisitorPtr& visitor)
    { visitor->visit( shared_from_this() ); }

} // namespace RE