#pragma once

#include <gk/gk++util.hpp>

namespace gk {
    // gk::GLStateProgramDataSet
    class GLStateProgramDataSet : public GLState {
        gk_program_data_set *_pds;

    public:
        GLStateProgramDataSet(gk_program_data_set *pds = nullptr) : _pds(pds) { }
        GLStateProgramDataSet(ProgramDataSet &pds) : GLStateProgramDataSet(&pds.pds) { }

        bool set(gk_program_data_set *pds, GLStateConfig &c) {
            if(_pds != pds || pds->dirty) {
                _pds = pds;
                c.dirty = true;
                return true;
            }
            return false;
        }

        bool set(ProgramDataSet &pds, GLStateConfig &c) {
            return set(&pds.pds, c);
        }

        virtual void apply(GLGlobalState &g) {
            g.useProgram(_pds->program);

            if(_pds->dirty & GK_PDS_DIRTY_UNIFORMS) applyUniformSet(_pds->uniforms);

            _pds->dirty = 0;
        }

        void applyUniformSet(gk_uniform_set *us) {
            for(size_t i = 0; i < us->nuniforms; ++i) {
                auto &v = us->values[i];

                if(v.count == 1) {
                    switch(v.type) {
                        case GK_UNIFORM_VALUE_FLOAT: glUniform1f(v.location,  v.value.f);  break;
                        case GK_UNIFORM_VALUE_INT:   glUniform1i(v.location,  v.value.i);  break;
                        case GK_UNIFORM_VALUE_UINT:  glUniform1ui(v.location, v.value.ui); break;
                        case GK_UNIFORM_VALUE_MAT4:
                            glUniformMatrix4fv(v.location, 1, false, (float*)v.value.data);
                            break;
                        default:
                            LOG("gk: bad uniform v type: ", v.type);
                            break;
                    }
                } else {
                    switch(v.type) {
                        case GK_UNIFORM_VALUE_FLOAT: glUniform1fv(v.location,  v.count, (float*)v.value.data); break;
                        case GK_UNIFORM_VALUE_INT:   glUniform1iv(v.location,  v.count, (int*)v.value.data); break;
                        case GK_UNIFORM_VALUE_UINT:  glUniform1uiv(v.location, v.count, (unsigned int*)v.value.data); break;
                        case GK_UNIFORM_VALUE_MAT4:
                            glUniformMatrix4fv(v.location, 1, false, (float*)v.value.data);
                            break;
                        default:
                            LOG("gk: bad uniform v type: ", v.type);
                            break;
                    }
                }
            }
        }
    };
}
