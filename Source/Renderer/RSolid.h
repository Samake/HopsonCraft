#ifndef RCHUNK_H_INCLUDED
#define RCHUNK_H_INCLUDED

#include <vector>

#include "../Shaders/Solid_Shader.h"
#include "RInfo.h"

struct Camera;

namespace Chunk
{
    class Section;
}

namespace Renderer
{
    class RChunk
    {
        public:
            void draw (const Chunk::Section& section);

            void update(const Camera& camera);

        private:
            void prepare(const Info& section);

        private:
            std::vector<Info> m_renderInfo;

            Shader::Solid_Shader m_shader;
    };
}

#endif // RCHUNK_H_INCLUDED
