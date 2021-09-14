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
  draw_scene(_fst_level_scene, _fst_level_idx);
  fst_level_vbox->addWidget(_fst_level_view);

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
      draw_scene(_fst_level_scene, idx);
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
  draw_scene(_scd_level_scene, _scd_level_idx);
  scd_level_vbox->addWidget(_scd_level_view);

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
      draw_scene(_scd_level_scene, idx);
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
  draw_scene(_output_level_scene, 0);

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

void AlignLevelDialog::draw_scene(QGraphicsScene* scene, const int level_idx)
{
  scene->clear();

  scene->addPixmap(_building.levels.at(level_idx).floorplan_pixmap);

  if (alignments.find(level_idx) != alignments.end() &&
    level_idx == _fst_level_idx)
  {
    draw_position(scene, alignments[level_idx].position);
    draw_orientation(scene, alignments[level_idx].position,
      alignments[level_idx].orientation);
  }

  if (alignments.find(_fst_level_idx) != alignments.end() &&
    alignments[_fst_level_idx].relative_point.find(level_idx) !=
    alignments[_fst_level_idx].relative_point.end() &&
    level_idx == _scd_level_idx)
  {
    draw_position(scene,
      alignments[_fst_level_idx].relative_point[level_idx].position);
    draw_orientation(scene,
      alignments[_fst_level_idx].relative_point[level_idx].position,
      alignments[_fst_level_idx].relative_point[level_idx].orientation);
  }
}

void AlignLevelDialog::draw_output_scene()
{

}

bool AlignLevelDialog::save(QString fn)
{
  QJsonArray jarr;
  QFile saveFile(fn);

  QJsonObject obj;

  // TODO : convert for loop to adapt multi map alignment
  // ------------------------------------------------------
  obj["Map"] = "right";
  QJsonArray arr_connection;
  QJsonObject connection;
  connection["To"] = "left";
  QJsonObject position;
  position["x"] = 0.0;
  position["y"] = 0.0;
  position["z"] = 0.0;
  connection["Position"] = position;
  QJsonObject orientation;
  orientation["x"] = 0.0;
  orientation["y"] = 0.0;
  orientation["w"] = 0.0;
  orientation["z"] = 0.0;
  connection["Orientation"] = orientation;
  arr_connection.append(connection);
  obj["Connection"] = arr_connection;
  jarr.append(obj);
  QJsonDocument doc(jarr);

  // ------------------------------------------------------

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

  //  0 : no one
  //  1 : _fst_level_view
  //  2 : _scd_level_view
  int selected_view = -1;

  if (is_mouse_event_in_map_view(e, p, _fst_level_view))
  {
    mv = _fst_level_view;
    selected_view = 1;
  }
  else if (is_mouse_event_in_map_view(e, p, _scd_level_view))
  {
    mv = _scd_level_view;
    selected_view = 2;
  }
  else
  {
    e->ignore();
    return;
  }

  switch (selected_view)
  {
    case 1:
      if (alignments.find(_fst_level_idx) != alignments.end())
      {
        return;
      }
      break;
    case 2:
      if (alignments.find(_fst_level_idx) == alignments.end() &&
        t == MOUSE_PRESS)
      {
        QMessageBox::critical(
          this,
          "Could not set relevant pose",
          "The reference level's origin hasn't set\n Set the origin in reference level first.",
          "Ok");
        return;
      }
      break;
    default:
      return;
  }

  if (t == MOUSE_PRESS)
  {
    if (!is_clicked)
    {
      draw_position(mv->scene(), p);

      clicked_point = p;
      is_clicked = true;
    }
    else
    {
      mv->scene()->removeItem(orientation_line);
      delete orientation_line;
      orientation_line = nullptr;

      if (selected_view == 1)
      {
        if (add_origin(clicked_point, p))
        {
          draw_scene(mv->scene(), _fst_level_idx);
        }
      }
      else
      {
        if (add_relative_point(clicked_point, p))
        {
          draw_scene(mv->scene(), _scd_level_idx);
        }
      }

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
  // draw_position(mv->scene(), p);
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

bool AlignLevelDialog::add_origin(const QPointF& start, const QPointF& dst)
{
  if (alignments.find(_fst_level_idx) != alignments.end())
  {
    return false;
  }

  AlignmentInfo align_info;
  align_info.idx = _fst_level_idx;
  align_info.name = _building.levels[_fst_level_idx].name;
  align_info.position = start;

  double diff_x = dst.x() - start.x();
  double diff_y = dst.y() - start.y();
  align_info.orientation = atan2(diff_y, diff_x);

  alignments.insert(std::pair<int, AlignmentInfo>(_fst_level_idx, align_info));

  return true;
}

bool AlignLevelDialog::add_relative_point(const QPointF& start,
  const QPointF& dst)
{
  if (alignments.find(_fst_level_idx) == alignments.end())
  {
    return false;
  }
  if (alignments[_fst_level_idx].relative_point.find(_scd_level_idx) !=
    alignments[_fst_level_idx].relative_point.end())
  {
    return false;
  }

  AlignmentInfo align_info;
  align_info.idx = _scd_level_idx;
  align_info.name = _building.levels[_scd_level_idx].name;

  align_info.position = start;

  double diff_x = dst.x() - start.x();
  double diff_y = dst.y() - start.y();
  align_info.orientation = atan2(diff_y, diff_x);

  alignments[_fst_level_idx].relative_point.insert(std::pair<int, AlignmentInfo>(
      _scd_level_idx,
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

void AlignLevelDialog::draw_position(QGraphicsScene* scene, const QPointF& p)
{
  double radius = 10;
  QPen position_pen(Qt::black);
  position_pen.setWidthF(radius / 2.0);

  const double a = 0.5;
  const QColor position_color = QColor::fromRgbF(0.0, 0.5, 0.0);
  QColor self_color(position_color);
  self_color.setAlphaF(a);

  QGraphicsEllipseItem* ellipse_item = scene->addEllipse(
    p.x() - 5,
    p.y() - 5,
    2 * 5,
    2 * 5,
    position_pen,
    self_color);

  ellipse_item->setZValue(20.0);
}

void AlignLevelDialog::draw_orientation(QGraphicsScene* scene, const QPointF& p,
  const double& angle)
{
  const double len = 20;
  double pen_width = 2;
  QColor color = QColor::fromRgbF(0, 0, 1, 0.5);

  QPen pen(QBrush(color), pen_width, Qt::SolidLine, Qt::PenCapStyle::RoundCap);

  double xx = p.x() + cos(angle) * len;
  double yy = p.y() + sin(angle) * len;

  QGraphicsLineItem* line_item = scene->addLine(p.x(), p.y(), xx, yy, pen);
}

void AlignLevelDialog::draw_moving_orientation(QGraphicsScene* scene,
  const QPointF& p,
  const double& angle)
{
  const double len = 20;
  double pen_width = 2;
  QColor color = QColor::fromRgbF(0, 0, 1, 0.5);

  QPen pen(QBrush(color), pen_width, Qt::SolidLine, Qt::PenCapStyle::RoundCap);

  double xx = p.x() + cos(angle) * len;
  double yy = p.y() + sin(angle) * len;

  if (orientation_line == nullptr)
  {
    orientation_line = scene->addLine(p.x(), p.y(),
        xx, yy, pen);
  }
  else
  {
    orientation_line->setLine(p.x(), p.y(),
      xx, yy);
  }
}