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

#include "align_level_dialog.h"
#include <QtWidgets>
#include <QSpacerItem>


AlignLevelDialog::AlignLevelDialog(Building& building)
: QDialog(), _building(building)
{
  //TODO : set window title
  setWindowTitle("TBD");
  _export_button = new QPushButton("Export", this);  // first button = [enter] button
  _cancel_button = new QPushButton("Cancel", this);

  QHBoxLayout* levels_graphic_hbox = new QHBoxLayout;

  // first level ui
  QVBoxLayout* fst_level_vbox = new QVBoxLayout;
  fst_level_vbox->addWidget(new QLabel("Level 1"));
  _fst_level_combo_box = new QComboBox;
  for (const auto& level : building.levels)
    _fst_level_combo_box->addItem(QString::fromStdString(level.name));
  fst_level_vbox->addWidget(_fst_level_combo_box);
  connect(
    _fst_level_combo_box,
    QOverload<int>::of(&QComboBox::currentIndexChanged),
    [this](const int idx)
    {
      draw_scene(_fst_level_scene, idx);
    });

  _fst_level_scene = new QGraphicsScene;
  _fst_level_view = new MapView;
  _fst_level_view->setScene(_fst_level_scene);

  draw_scene(_fst_level_scene, 0);

  fst_level_vbox->addWidget(_fst_level_view);

  // second level ui
  QVBoxLayout* scd_level_vbox = new QVBoxLayout;
  scd_level_vbox->addWidget(new QLabel("Level 2"));

  _scd_level_combo_box = new QComboBox;
  for (const auto& level : building.levels)
    _scd_level_combo_box->addItem(QString::fromStdString(level.name));
  scd_level_vbox->addWidget(_scd_level_combo_box);
  connect(
    _scd_level_combo_box,
    QOverload<int>::of(&QComboBox::currentIndexChanged),
    [this](const int idx)
    {
      draw_scene(_scd_level_scene, idx);
    });

  _scd_level_scene = new QGraphicsScene;
  _scd_level_view = new MapView;
  _scd_level_view->setScene(_scd_level_scene);
  draw_scene(_scd_level_scene, 1);

  scd_level_vbox->addWidget(_scd_level_view);

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

}

void AlignLevelDialog::draw_scene(QGraphicsScene* scene, const int level_idx)
{
  scene->clear();

  scene->addPixmap(_building.levels.at(level_idx).floorplan_pixmap);
}

void AlignLevelDialog::draw_output_scene()
{

}

