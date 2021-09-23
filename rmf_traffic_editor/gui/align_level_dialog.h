/*
 * Copyright (C) 2019-2020 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef ALIGN_LEVEL_DIALOG_H
#define ALIGN_LEVEL_DIALOG_H

#include <QDialog>
#include <QObject>

#include "building.h"
#include "align_level_table.h"
#include "map_view.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QMouseEvent;
QT_END_NAMESPACE

struct AlignmentInfo
{
  int idx = -1;
  std::string name = "";
  QPointF position;
  double orientation;
  QColor color;

  /// TODO: add link infomation with other levels.
  std::map<int, AlignmentInfo> relative_point;
};


class AlignLevelDialog : public QDialog
{
  Q_OBJECT

public:
  AlignLevelDialog(Building& building);
  ~AlignLevelDialog();

protected:
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);

private:
  Building& _building;

  std::map<int, AlignmentInfo> _alignments;

  QLineEdit* _building_name_line_edit;
  QComboBox* _reference_floor_combo_box;
  QComboBox* _fst_level_combo_box;
  QComboBox* _scd_level_combo_box;

  int _fst_level_idx = 0;
  int _scd_level_idx = 1;

  QGraphicsScene* _fst_level_scene;
  MapView* _fst_level_view;

  QGraphicsScene* _scd_level_scene;
  MapView* _scd_level_view;

  QGraphicsScene* _output_level_scene;
  MapView* _output_level_view;

  MapView* _last_mv;

  AlignLevelTable* _align_table;

  QPushButton* _export_button, * _cancel_button;

  QColor _color;
  std::vector<QColor> _pallete;


  void draw_scene(QGraphicsScene* scene,
    const int draw_level_idx);
  void draw_output_scene();
  bool save(QString fn);

  // for mouse event
  enum MouseType
  {
    MOUSE_UNDEFINED = 0,
    MOUSE_PRESS = 1,
    MOUSE_RELEASE = 2,
    MOUSE_MOVE = 3
  };

  bool _is_clicked = false;
  bool _is_ctrl_pressed = false;
  QPointF _clicked_point;
  QGraphicsLineItem* _orientation_line_x = nullptr;
  QGraphicsLineItem* _orientation_line_y = nullptr;
  QGraphicsSimpleTextItem* _position_text = nullptr;

  void mouse_event(const MouseType t, QMouseEvent* e);
  bool is_mouse_event_in_map_view(QMouseEvent* e, QPointF& p, MapView* mv);

  bool is_have_origin(const int level_idx);
  bool is_have_relative(const int selected_idx, const int relative_idx);

  bool add_origin(const Level& level, const int idx);
  bool add_origin(const QPointF& origin, const QPointF& dst, const int idx,
    const QColor color);
  bool add_relative_point(const QPointF& start, const QPointF& dst,
    const int from_idx, const int to_idx);

  // for graphics
  void draw_position(QGraphicsScene* scene, const QPointF& p, QColor color);
  void draw_orientation(QGraphicsScene* scene,
    const QPointF& p, const double& angle);
  void draw_moving_orientation(QGraphicsScene* scene,
    const QPointF& p, const double& angle);
  QColor gen_random_color();

private slots:
  void export_button_clicked();
  void ok_button_clicked();
};

#endif
