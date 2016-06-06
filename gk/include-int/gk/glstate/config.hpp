#pragma once

namespace gk {
    // gk::GLStateConfig
    class GLStateConfig {
    public:
        bool dirty = true;
        virtual void apply(GLGlobalState&) = 0;
        virtual void clean(GLGlobalState&) = 0;
    };

    // gk::GLStateConfigGeneral
    typedef std::vector<GLState*> StateVector;
    class GLStateConfigGeneral : public GLStateConfig {
    protected:
        StateVector states;
    public:
        inline virtual void apply(GLGlobalState &g) {
            if(dirty) {
                for(auto *state : states)
                    state->apply(g);
                dirty = false;
            }
        }

        inline virtual void clean(GLGlobalState &g) {
            for(auto *state : states)
                state->clean(g);
            dirty = true;
        }

        template<typename...Rest>
        inline void add(GLState &s, Rest&...args) {
            states.push_back(&s);
            add(args...);
        }

        inline void add() {
            dirty = true;
        }
    };
}
