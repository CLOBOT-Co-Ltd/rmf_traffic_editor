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

  std::map<int, AlignmentInfo> alignments;

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

  AlignLevelTable* _align_table;

  QPushButton* _export_button, * _cancel_button;

  void draw_scene(QGraphicsScene* scene, const int level_idx);
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

  bool is_clicked = false;
  QPointF clicked_point;
  QGraphicsEllipseItem* postion_ellipse = nullptr;
  QGraphicsLineItem* orientation_line = nullptr;

  void mouse_event(const MouseType t, QMouseEvent* e);
  bool is_mouse_event_in_map_view(QMouseEvent* e, QPointF& p, MapView* mv);

  bool add_origin(const QPointF& start, const QPointF& dst);
  bool add_relative_point(const QPointF& start, const QPointF& dst);

  // for graphics
  void draw_position(QGraphicsScene* scene, const QPointF& p);
  void draw_orientation(QGraphicsScene* scene, const QPointF& p,
    const double& angle);
  void draw_moving_orientation(QGraphicsScene* scene, const QPointF& p,
    const double& angle);

private slots:
  void export_button_clicked();
  void ok_button_clicked();
};

#endif
