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
class QLineEdit;
class QComboBox;


class AlignLevelDialog : public QDialog
{
public:
  AlignLevelDialog(Building& building);
  ~AlignLevelDialog();

private:
  Building& _building;

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

private slots:
  void export_button_clicked();
  void ok_button_clicked();
};

#endif
