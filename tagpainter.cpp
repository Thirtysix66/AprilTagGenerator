#include "tagpainter.h"
// clang-format off
#include <AprilTags/TagDetector.h>
#include <AprilTags/Tag36h11.h>
// clang-format on
#include <stack>

static AprilTags::TagCodes tag_codes = AprilTags::tagCodes36h11;
static Qt::GlobalColor fore_color = Qt::white;
static Qt::GlobalColor back_color = Qt::black;

TagPainter::TagPainter(unsigned long start_id, int border, int corner_box,
                       int padding_pixels, int cols, int rows,
                       int unit_pixels) :
  start_id_(start_id), border_(border), corner_box_(corner_box),
  padding_pixels_(padding_pixels), cols_(cols), rows_(rows),
  unit_pixels_(unit_pixels)
{
  // the tagcode size
  codes_size_ = static_cast<int>(std::sqrt(tag_codes.bits));
  // calculate parameters
  tag_size_ = static_cast<int>(border_ * 2 + codes_size_);
  total_width_ = tag_size_ * cols_ + corner_box_ * (cols_ + 1);
  total_height_ = tag_size_ * rows_ + corner_box_ * (rows_ + 1);
}

void TagPainter::PaintTagAt(QPainter &painter, unsigned long id, int x, int y)
{
  painter.save();

  // painter.setClipRect(QRect(x, y, tag_size_, tag_size_));
  // draw the entire tag with black
  painter.translate(x, y);
  int real_tag_size = tag_size_ * unit_pixels_;
  painter.fillRect(QRect(0, 0, real_tag_size, real_tag_size), back_color);
  // draw the codes area with white
  painter.translate(border_ * unit_pixels_, border_ * unit_pixels_);
  int real_codes_size = codes_size_ * unit_pixels_;
  painter.fillRect(QRect(0, 0, real_codes_size, real_codes_size), fore_color);

  // paint according to the tag_id
  int i = 0;
  unsigned long long code = tag_codes.codes[id];
  std::stack<bool> code_stack;
  while (i != tag_codes.bits) {
    code_stack.push(code & 1);
    i++;
    code >>= 1;
  }
  for (int col = 0; col < codes_size_; col++) {
    for (int row = 0; row < codes_size_; row++) {
      // fill the pixel with while if the code bit is true
      painter.fillRect(QRect(row * unit_pixels_, col * unit_pixels_,
                             unit_pixels_, unit_pixels_),
                             code_stack.top() ? fore_color : back_color);
      code_stack.pop();
    }
  }

  painter.restore();
}

void TagPainter::PaintTagGrid(QPainter &painter)
{
  painter.save();

  // paint the entire working rectangle
  painter.fillRect(QRect(0, 0,
                         total_width_ * unit_pixels_ + padding_pixels_ * 2,
                         total_height_ * unit_pixels_ + padding_pixels_ * 2),
                   fore_color);
  // paint the tags from the start id to the end
  auto id = start_id_;
  for (int j = rows_ - 1; j >= 0; --j) {
    for (int i = 0; i < cols_; ++i) {
      int x = padding_pixels_ +
              unit_pixels_ * ((i + 1) * corner_box_ + i * tag_size_);
      int y = padding_pixels_ +
              unit_pixels_ * ((j + 1) * corner_box_ + j * tag_size_);
      PaintTagAt(painter, id, x, y);
      id++;
    }
  }

  painter.restore();
}

void TagPainter::PaintCornerGrid(QPainter &painter)
{
  painter.save();

  // paint the corner box grids
  for (int i = 0; i < cols_ + 1; ++i) {
    for (int j = 0; j < rows_ + 1; ++j) {
      int x = padding_pixels_ + unit_pixels_ * (i * (corner_box_ + tag_size_));
      int y = padding_pixels_ + unit_pixels_ * (j * (corner_box_ + tag_size_));
      painter.fillRect(QRect(x, y,
                             corner_box_ * unit_pixels_,
                             corner_box_ * unit_pixels_),
                       back_color);
    }
  }
  painter.restore();
}

void TagPainter::PaintTagBoard(QPainter &painter)
{
  PaintTagGrid(painter);
  PaintCornerGrid(painter);
}
