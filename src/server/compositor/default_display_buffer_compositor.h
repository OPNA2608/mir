/*
 * Copyright © Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 or 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MIR_COMPOSITOR_DEFAULT_DISPLAY_BUFFER_COMPOSITOR_H_
#define MIR_COMPOSITOR_DEFAULT_DISPLAY_BUFFER_COMPOSITOR_H_

#include "mir/compositor/display_buffer_compositor.h"
#include "mir/compositor/compositor_report.h"
#include <memory>

namespace mir
{
namespace graphics
{
class DisplayBuffer;
}
namespace renderer
{
class Renderer;
}
namespace compositor
{

class Scene;

class DefaultDisplayBufferCompositor : public DisplayBufferCompositor
{
public:
    DefaultDisplayBufferCompositor(
        graphics::DisplayBuffer& display_buffer,
        std::shared_ptr<renderer::Renderer> const& renderer,
        std::shared_ptr<CompositorReport> const& report);

    void composite(SceneElementSequence&& scene_sequence) override;

private:
    graphics::DisplayBuffer& display_buffer;
    std::shared_ptr<renderer::Renderer> const renderer;
    std::shared_ptr<CompositorReport> const report;
};

}
}

#endif /* MIR_COMPOSITOR_DEFAULT_DISPLAY_BUFFER_COMPOSITOR_H_ */
