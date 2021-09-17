/*
 * Copyright (C) 2019-2020 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, sosetMouseTrackingftware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <QtWidgets>
#include <QSpacerItem>
#include <QJsonObject>
#include <QJsonDocument>

#include "align_level_dialog.h"

AlignLevelDialog::AlignLevelDialog(Building& building)
: QDialog(), _building(building)
{
  //TODO : set window title
  setWindowTitle("TBD");
  setMouseTracking(true);
  _export_button = new QPushButton("Export", this);  // first button = [enter] button
  _cancel_button = new QPushButton("Cancel", this);

  // add origin in alignments
  for (size_t i = 0; i < _building.levels.size(); i++)
  {
    add_origin(_building.levels[i], i);
  }

  QHBoxLayout* levels_graphic_hbox = new QHBoxLayout;

  // first level ui
  QVBoxLayout* fst_level_vbox = new QVBoxLayout;
  fst_level_vbox->addWidget(new QLabel("Level 1"));

  _fst_level_combo_box = new QComboBox;
  for (const auto& level : building.levels)
    _fst_level_combo_box->addItem(QString::fromStdString(level.name));
  _fst_level_idx = 0;
  _fst_level_combo_box->setCurrentIndex(_fst_level_idx);
  fst_level_vbox->addWidget(_fst_level_combo_box);

  _fst_level_scene = new QGraphicsScene;
  _fst_level_view = new MapView;
  _fst_level_view->setScene(_fst_level_scene);
  draw_scene(_fst_level_scene, _fst_level_idx, _scd_level_idx);
  fst_level_vbox->addWidget(_fst_level_view);
  _fst_level_view->scale(1.00001, 1.00001);

  connect(
    _fst_level_combo_box,
    QOverload<int>::of(&QComboBox::currentIndexChanged),
    [this](const int idx)
    {
      int old_fst_level_idx = _fst_level_idx;
      _fst_level_idx = idx;
      if (idx == _scd_level_idx)
      {
        _scd_level_combo_box->setCurrentIndex(old_fst_level_idx);
      }
      draw_scene(_fst_level_scene, idx, old_fst_level_idx);
    });

  // second level ui
  QVBoxLayout* scd_level_vbox = new QVBoxLayout;
  scd_level_vbox->addWidget(new QLabel("Level 2"));

  _scd_level_combo_box = new QComboBox;
  for (const auto& level : building.levels)
    _scd_level_combo_box->addItem(QString::fromStdString(level.name));
  _scd_level_idx = 1;
  _scd_level_combo_box->setCurrentIndex(_scd_level_idx);
  scd_level_vbox->addWidget(_scd_level_combo_box);

  _scd_level_scene = new QGraphicsScene;
  _scd_level_view = new MapView;
  _scd_level_view->setScene(_scd_level_scene);
  draw_scene(_scd_level_scene, _scd_level_idx, _fst_level_idx);
  scd_level_vbox->addWidget(_scd_level_view);
  _scd_level_view->scale(1.00001, 1.00001);

  connect(
    _scd_level_combo_box,
    QOverload<int>::of(&QComboBox::currentIndexChanged),
    [this](const int idx)
    {
      if (idx == _fst_level_idx)
      {
        _scd_level_combo_box->setCurrentIndex(_scd_level_idx);
        return;
      }
      _scd_level_idx = idx;
      draw_scene(_scd_level_scene, idx, _fst_level_idx);
    });


  // output level ui
  QVBoxLayout* output_level_vbox = new QVBoxLayout;
  output_level_vbox->addWidget(new QLabel("output"));
  output_level_vbox->addItem(
    new QSpacerItem(
      _scd_level_combo_box->sizeHint().width(),
      _scd_level_combo_box->sizeHint().height(),
      QSizePolicy::Minimum,
      QSizePolicy::Preferred));

  _output_level_scene = new QGraphicsScene;
  _output_level_view = new MapView;
  _output_level_view->setScene(_output_level_scene);

  output_level_vbox->addWidget(_output_level_view);

  levels_graphic_hbox->addLayout(fst_level_vbox);
  levels_graphic_hbox->addLayout(scd_level_vbox);
  levels_graphic_hbox->addLayout(output_level_vbox);

  QHBoxLayout* table_hbox = new QHBoxLayout;
  _align_table = new AlignLevelTable;
  _align_table->setMinimumSize(400, 100);
  _align_table->setMaximumHeight(100);
  _align_table->setSizePolicy(QSizePolicy::MinimumExpanding,
    QSizePolicy::MinimumExpanding);
  table_hbox->addWidget(_align_table);

  QHBoxLayout* bottom_buttons_hbox = new QHBoxLayout;
  bottom_buttons_hbox->addWidget(_cancel_button);
  bottom_buttons_hbox->addWidget(_export_button);
  connect(
    _export_button, &QAbstractButton::clicked,
    this, &AlignLevelDialog::export_button_clicked);
  connect(
    _cancel_button, &QAbstractButton::clicked,
    this, &QDialog::reject);

  QVBoxLayout* top_vbox = new QVBoxLayout;

  top_vbox->addLayout(levels_graphic_hbox);
  top_vbox->addLayout(table_hbox);
  // todo: some sort of separator (?)
  top_vbox->addLayout(bottom_buttons_hbox);

  setLayout(top_vbox);
}

AlignLevelDialog::~AlignLevelDialog()
{
}

void AlignLevelDialog::ok_button_clicked()
{
  _building.name = _building_name_line_edit->text().toStdString();
  _building.reference_level_name =
    _reference_floor_combo_box->currentText().toStdString();
  accept();
}

void AlignLevelDialog::export_button_clicked()
{
  QFileDialog dialog(this, "Export as json");
  dialog.setNameFilter("*.json");
  dialog.setDefaultSuffix(".json");
  dialog.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
  dialog.setConfirmOverwrite(true);

  if (dialog.exec() != QDialog::Accepted)
    return;

  QFileInfo file_info(dialog.selectedFiles().first());

  save(file_info.absoluteFilePath());
}

void AlignLevelDialog::draw_scene(QGraphicsScene* scene,
  const int draw_level_idx,
  const int relative_level_idx)
{
  scene->clear();

  scene->addPixmap(_building.levels.at(draw_level_idx).floorplan_pixmap);

  QColor p_color = QColor::fromRgbF(0.0, 0.5, 0.0, 0.5);

  if (is_have_origin(draw_level_idx))
  {
    draw_position(scene, alignments[draw_level_idx].position,
      alignments[draw_level_idx].color);
    draw_orientation(scene, alignments[draw_level_idx].position,
      alignments[draw_level_idx].orientation);
    QGraphicsSimpleTextItem* item = scene->addSimpleText(
      QString::fromStdString("origin"));
    item->setBrush(alignments[draw_level_idx].color);
    item->setPos(alignments[draw_level_idx].position.x(),
      alignments[draw_level_idx].position.y() + 10);

    for (const auto to : alignments[draw_level_idx].relative_point)
    {
      QColor p_color = QColor::fromHsvF(
        alignments[to.second.idx].color.hsvHueF(), 0.3, 0.99);
      draw_position(scene, to.second.position, p_color);
      draw_orientation(scene, to.second.position,
        to.second.orientation);


      QGraphicsSimpleTextItem* item = scene->addSimpleText(
        QString::fromStdString(alignments[to.second.idx].name));
      item->setBrush(alignments[to.second.idx].color);
      item->setPos(to.second.position.x(),
        to.second.position.y() + 10);
    }
  }
}

void AlignLevelDialog::draw_output_scene()
{
  // TODO : Draw output scene which contains combined image.
}

bool AlignLevelDialog::save(QString fn)
{
  QJsonArray jarr;
  QFile saveFile(fn);

  QJsonObject obj;

  for (const auto a : alignments)
  {
    QJsonObject obj;

    obj["Map"] = a.second.name.c_str();
    QJsonArray arr_connection;
    for (const auto to : a.second.relative_point)
    {
      QJsonObject connection;
      connection["To"] = to.second.name.c_str();
      QJsonObject position;
      double real_x_src = (to.second.position.x() - a.second.position.x());
      real_x_src *= _building.level_meters_per_pixel(a.second.name);

      double real_y_src = -1 * (to.second.position.y() - a.second.position.y());
      real_y_src *= _building.level_meters_per_pixel(a.second.name);

      double rotate_x;
      double rotate_y;
      // * the orientaion of alignmentinfo is aimed to origin of image(graphic scene)
      // * To follow ros coordinate system, multiply '-1' to orientation
      double yaw = -a.second.orientation;

      rotate_x = cos(-yaw) * real_x_src - sin(-yaw) * real_y_src;
      rotate_y = sin(-yaw) * real_x_src + cos(-yaw) * real_y_src;

      double diff_yaw = -1*to.second.orientation - yaw;

      // * QQuaternion's fromEulerAngles function has different coordinate system from ros.
      // * input yaw to roll (z axis around).
      QQuaternion quaternion = QQuaternion::fromEulerAngles(0., 0., (float)diff_yaw * 180 / M_PI);
      
      position["x"] = rotate_x;
      position["y"] = rotate_y;
      position["z"] = 0.0;
      connection["Position"] = position;
      QJsonObject orientation;
      orientation["w"] = quaternion.scalar();
      orientation["x"] = quaternion.x();
      orientation["y"] = quaternion.y();
      orientation["z"] = quaternion.z();
      connection["Orientation"] = orientation;
      arr_connection.append(connection);

    }
    obj["Connection"] = arr_connection;
    jarr.append(obj);
  }

  QJsonDocument doc(jarr);

  if (!saveFile.open(QIODevice::WriteOnly))
  {
    return false;
  }

  saveFile.write(doc.toJson());

  return true;
}

void AlignLevelDialog::mouse_event(const MouseType t, QMouseEvent* e)
{
  QPointF p;
  MapView* mv;

  int selected_level_idx = -1;
  int relative_level_idx = -1;

  if (is_mouse_event_in_map_view(e, p, _fst_level_view))
  {
    mv = _fst_level_view;
    selected_level_idx = _fst_level_idx;
    relative_level_idx = _scd_level_idx;
  }
  else if (is_mouse_event_in_map_view(e, p, _scd_level_view))
  {
    mv = _scd_level_view;
    selected_level_idx = _scd_level_idx;
    relative_level_idx = _fst_level_idx;
  }
  else
  {
    // * mouse point moving out the scene, remove text
    if (position_text != nullptr)
    {
      _last_mv->scene()->removeItem(position_text);
      delete position_text;
      position_text = nullptr;
    }

    e->ignore();
    return;
  }

  // draw text around mouse point
  if (_last_mv != mv)
  {
    if (position_text != nullptr)
    {
      _last_mv->scene()->removeItem(position_text);
      delete position_text;
      position_text = nullptr;
    }
  }
  if (!is_clicked)
  {
    std::string name = _building.levels[selected_level_idx].name;

    double y_offset;
    double real_x_src;
    double real_y_src;
    QString text;

    if (is_have_origin(selected_level_idx))
    {
      y_offset = (double)_building.levels[selected_level_idx].drawing_height;
      real_x_src = (p.x() - alignments[selected_level_idx].position.x());
      real_x_src *= _building.level_meters_per_pixel(name);

      real_y_src = -1 * (p.y() - alignments[selected_level_idx].position.y());
      real_y_src *= _building.level_meters_per_pixel(name);

      double rotate_x;
      double rotate_y;
      // the orientaion of alignmentinfo is aimed to origin of image(graphic scene)
      // To follow ros coordinate system, multiply '-1' to orientation
      double yaw = -alignments[selected_level_idx].orientation;

      rotate_x = cos(-yaw) * real_x_src - sin(-yaw) * real_y_src;
      rotate_y = sin(-yaw) * real_x_src + cos(-yaw) * real_y_src;

      text.sprintf("Set pose\nx : %3.2f\ny : %3.2f",
        rotate_x, rotate_y);
    }
    else
    {
      y_offset = (double)_building.levels[selected_level_idx].drawing_height;
      real_x_src = -1 * p.x() * _building.level_meters_per_pixel(name);
      real_y_src = (p.y() - y_offset) * _building.level_meters_per_pixel(name);
      text.sprintf("Set origin\nx : %3.2f\n y : %3.2f",
        real_x_src, real_y_src);
    }

    if (position_text == nullptr)
    {
      position_text = mv->scene()->addSimpleText(text);
      _last_mv = mv;
      position_text->setBrush(QColor(0, 0, 0));
      position_text->setPos(p.x(), p.y() + 15);
    }
    else
    {
      position_text->setText(text);
      position_text->setPos(p.x(), p.y() + 15);
    }
  }

  // do the job
  if (t == MOUSE_PRESS)
  {
    if (!is_clicked)
    {
      is_ctrl_pressed = QGuiApplication::keyboardModifiers().testFlag(
        Qt::ControlModifier);

      if (!is_ctrl_pressed)
      {
        if (!is_have_origin(relative_level_idx) ||
          !is_have_origin(selected_level_idx))
        {
          QMessageBox::critical(
            this,
            "Could not set relevant pose",
            "Levels origin hasn't set\n Set the origin in level first.",
            "Ok");
          return;
        }
        // if already have relationship
        if (is_have_relative(selected_level_idx, relative_level_idx))
        {
          return;
        }
      }
      else
      {
        if (is_have_origin(selected_level_idx))
        {
          return;
        }
      }

      _color = gen_random_color();
      draw_position(mv->scene(), p, _color);

      clicked_point = p;
      is_clicked = true;
    }
    else
    {
      mv->scene()->removeItem(orientation_line_x);
      mv->scene()->removeItem(orientation_line_y);
      delete orientation_line_x;
      delete orientation_line_y;
      orientation_line_x = nullptr;
      orientation_line_y = nullptr;
      if (position_text != nullptr)
      {
        mv->scene()->removeItem(position_text);
        delete position_text;
        position_text = nullptr;
      }

      if (is_ctrl_pressed)
      {
        if (add_origin(clicked_point, p, selected_level_idx, _color))
        {
          draw_scene(mv->scene(), selected_level_idx, relative_level_idx);
        }
      }
      else
      {
        if (add_relative_point(clicked_point, p, selected_level_idx,
          relative_level_idx))
        {
          draw_scene(mv->scene(), selected_level_idx, relative_level_idx);
        }
      }

      is_ctrl_pressed = false;
      is_clicked = false;
    }
  }
  else if (t == MOUSE_MOVE && is_clicked)
  {
    double diff_x = p.x() - clicked_point.x();
    double diff_y = p.y() - clicked_point.y();
    double angle = atan2(diff_y, diff_x);
    draw_moving_orientation(mv->scene(), clicked_point, angle);
  }
}

bool AlignLevelDialog::is_mouse_event_in_map_view(QMouseEvent* e, QPointF& p,
  MapView* mv)
{
  const QPoint p_global = mapToGlobal(e->pos());
  const QPoint p_map = mv->mapFromGlobal(p_global);

  if (p_map.x() < 0 || p_map.y() < 0 ||
    p_map.x() >= mv->width() || p_map.y() >= mv->height())
    return false;
  // This event point is valid. Now we can set p_scene.
  p = mv->mapToScene(p_map);
  return true;
}

bool AlignLevelDialog::is_have_origin(const int level_idx)
{
  return alignments.find(level_idx) != alignments.end();
}

bool AlignLevelDialog::is_have_relative(const int from_idx,
  const int to_idx)
{
  if (!is_have_origin(from_idx))
  {
    return false;
  }

  return alignments[from_idx].relative_point.find(to_idx) !=
    alignments[from_idx].relative_point.end();
}

bool AlignLevelDialog::add_origin(const Level& level, const int idx)
{
  if (is_have_origin(idx))
  {
    return false;
  }

  AlignmentInfo align_info;
  align_info.idx = idx;
  align_info.name = level.name;
  align_info.position.setX(((double)level.drawing_width)/2);
  align_info.position.setY(((double)level.drawing_height)/2);

  align_info.orientation = 0;

  align_info.color = gen_random_color();

  alignments.insert(std::pair<int, AlignmentInfo>(idx, align_info));

  return true;
}
bool AlignLevelDialog::add_origin(const QPointF& start, const QPointF& dst,
  const int idx, const QColor color)
{
  if (is_have_origin(idx))
  {
    return false;
  }

  AlignmentInfo align_info;
  align_info.idx = idx;
  align_info.name = _building.levels[idx].name;
  align_info.position = start;

  double diff_x = dst.x() - start.x();
  double diff_y = dst.y() - start.y();
  align_info.orientation = atan2(diff_y, diff_x);

  align_info.color = color;

  alignments.insert(std::pair<int, AlignmentInfo>(idx, align_info));

  return true;
}

bool AlignLevelDialog::add_relative_point(const QPointF& start,
  const QPointF& dst, const int from_idx, const int to_idx)
{
  if (!is_have_origin(from_idx))
  {
    return false;
  }

  if (is_have_relative(from_idx, to_idx))
  {
    return false;
  }

  AlignmentInfo align_info;
  align_info.idx = to_idx;
  align_info.name = _building.levels[to_idx].name;

  align_info.position = start;

  double diff_x = dst.x() - start.x();
  double diff_y = dst.y() - start.y();
  align_info.orientation = atan2(diff_y, diff_x);

  alignments[from_idx].relative_point.insert(std::pair<int, AlignmentInfo>(
      to_idx,
      align_info));
  return true;
}

void AlignLevelDialog::mousePressEvent(QMouseEvent* e)
{
  mouse_event(MOUSE_PRESS, e);
}

void AlignLevelDialog::mouseMoveEvent(QMouseEvent* e)
{
  mouse_event(MOUSE_MOVE, e);
}

void AlignLevelDialog::draw_position(QGraphicsScene* scene, const QPointF& p,
  QColor color)
{
  double radius = 10;
  QPen position_pen(Qt::black);
  position_pen.setWidthF(radius / 5.0);

  QGraphicsEllipseItem* ellipse_item = scene->addEllipse(
    p.x() - 5,
    p.y() - 5,
    2 * 5,
    2 * 5,
    position_pen,
    color);
}

void AlignLevelDialog::draw_orientation(QGraphicsScene* scene,
  const QPointF& p, const double& angle)
{
  const double len = 20;
  double pen_width = 2.5;

  QColor r = QColor::fromRgbF(1.0, 0.0, 0.0, 1.0);
  QColor g = QColor::fromRgbF(0.0, 1.0, 0.0, 1.0);

  QPen pen_g(QBrush(g), pen_width, Qt::SolidLine, Qt::PenCapStyle::RoundCap);
  QPen pen_r(QBrush(r), pen_width, Qt::SolidLine, Qt::PenCapStyle::RoundCap);
  double xx = p.x() + cos(angle) * len;
  double yy = p.y() + sin(angle) * len;

  QGraphicsLineItem* line_item_x = scene->addLine(p.x(), p.y(), xx, yy, pen_r);

  xx = p.x() + cos(angle - M_PI_2) * len;
  yy = p.y() + sin(angle - M_PI_2) * len;
  QGraphicsLineItem* line_item_y = scene->addLine(p.x(), p.y(), xx, yy, pen_g);
}

void AlignLevelDialog::draw_moving_orientation(QGraphicsScene* scene,
  const QPointF& p, const double& angle)
{
  const double len = 20;
  double pen_width = 2.5;

  QColor r = QColor::fromRgbF(1.0, 0.0, 0.0, 1.0);
  QColor g = QColor::fromRgbF(0.0, 1.0, 0.0, 1.0);


  QPen pen_g(QBrush(g), pen_width, Qt::SolidLine, Qt::PenCapStyle::RoundCap);
  QPen pen_r(QBrush(r), pen_width, Qt::SolidLine, Qt::PenCapStyle::RoundCap);

  double xx = p.x() + cos(angle) * len;
  double yy = p.y() + sin(angle) * len;

  if (orientation_line_x == nullptr)
  {
    orientation_line_x = scene->addLine(p.x(), p.y(),
        xx, yy, pen_r);
  }
  else
  {
    orientation_line_x->setLine(p.x(), p.y(),
      xx, yy);
  }

  xx = p.x() + cos(angle - M_PI_2) * len;
  yy = p.y() + sin(angle - M_PI_2) * len;

  if (orientation_line_y == nullptr)
  {
    orientation_line_y = scene->addLine(p.x(), p.y(),
        xx, yy, pen_g);
  }
  else
  {
    orientation_line_y->setLine(p.x(), p.y(),
      xx, yy);
  }
}

QColor AlignLevelDialog::gen_random_color()
{
  const double golden_ratio_conjugate = 0.618033988749895;
  double r = ((double)rand() / RAND_MAX) + 0.618033988749895;
  r = fmod(r, 1.0);
  double g = ((double)rand() / RAND_MAX) + 0.618033988749895;
  g = fmod(g, 1.0);
  double b = ((double)rand() / RAND_MAX) + 0.618033988749895;
  b = fmod(b, 1.0);

  return QColor::fromRgbF(r, g, b);
}