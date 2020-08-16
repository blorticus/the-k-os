# Sketch ideas for the project

## The initial termainal

The initial terminal should assume that it is a frame buffer of 24 bit depth with an arbitrary width and height.  On this can be drawn primitive items: a line (of a desired color) and a rectange, initially.  In addition, and arbitary bitmap can be rendered.  It can be full color or two-tone (background/foreground).

From this initial fb terminal, build a text terminal.  It uses the two-tone bitmap to render runes (unicode code points as 32-bit unsigned) from a fixed-size font definition.

For the drawing FrameBuffer:

```c++
fb = new FrameBuffer::FrameBuffer((Memptr)fb_start_of_memory, 1024, 768)
fb.AddALine().From(FrameBuffer::Point{0, 0}).To(FrameBuffer::Point{10, 10}).StrokeColor(FrameBuffer::Color::Blue)
fb.AddARectangle().UpperLeft(FrameBuffer::Point{10,10}).LowerRight(FrameBuffer::Point{20,20}).NoStroke().FillColor(FrameBuffer::Color::Green)
fb.AddBitmap().UpperLeft(FrameBuffer::Point{50,10}).FromSource(new FrameBuffer::TwoToneBitmap(8, 16, [][]bitmap_definition))
fb.Render()

fb = new FrameBuffer::FrameBuffer((Memptr)fb_start_of_memory, 1024, 768, FrameBuffer::FrameBuffer::RenderDirectlyToFrameBuffer)
```

```c++
namespace FrameBuffer
class FrameBuffer::Point {
public:
  uint32 x, y
}

type FrameBuffer::ColorType uint32
enum FrameBuffer::Color: FrameBuffer::ColorType {
  White = 0
  Black = 0x00ffffff
  ...
}

class FrameBuffer::FrameBuffer {
public:
  FrameBuffer(Memptr memoryLocationOfUpperLeftPixel, Uint32 widthInPixels, Uint32 heightInPixels);
  FrameBuffer(Memptr memoryLocationOfUpperLeftPixel, Uint32 widthInPixels, Uint32 heightInPixels, FrameBufferAttributes attributes);
  ClearVisibleScreen();
  DrawALine(FrameBuffer::Point from, FrameBuffer::Point to, FrameBuffer::ColorType StrokeColor);
}

class FrameBuffer::Line {
private:
  Line(FrameBuffer containingFrameBuffer);
public:
  Draw(FrameBuffer::Point from, FrameBuffer::Point to);
}
```
