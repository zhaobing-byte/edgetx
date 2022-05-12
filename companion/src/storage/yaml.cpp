/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "yaml.h"
#include "edgetxinterface.h"
#include "eeprominterface.h"
#include "yaml_ops.h"

#include <QFile>
#include <QDir>

bool YamlFormat::loadFile(QByteArray & filedata)
{
  QString path = filename;
  QFile file(path);
  if (!file.open(QFile::ReadOnly)) {
    setError(tr("Error opening file %1:\n%2.").arg(path).arg(file.errorString()));
    return false;
  }
  filedata = file.readAll();
  qDebug() << "File" << path << "read, size:" << filedata.size();
  file.close();
  return true;
}

bool YamlFormat::writeFile(const QByteArray & filedata, const QString & filename)
{
  QString path = this->filename + "/" + filename;
  QFile file(path);
  if (!file.open(QFile::WriteOnly)) {
    setError(tr("Error opening file %1 in write mode:\n%2.").arg(path).arg(file.errorString()));
    return false;
  }
  file.write(filedata.data(), filedata.size());
  file.close();
  qDebug() << "File" << path << "written, size:" << filedata.size();
  return true;
}

bool YamlFormat::load(RadioData & radioData)
{
  bool hasCategories = getCurrentFirmware()->getCapability(HasModelCategories);
  int modelIdx = 0;

  QByteArray data;
  if (!loadFile(data)) {
    setError(tr("Cannot read %1").arg(filename));
    return false;
  }

  std::istringstream data_istream(data.toStdString());
  YAML::Node node = YAML::Load(data_istream);

  board = getCurrentBoard();

  if (!node.IsMap()) {
    setError(tr("File %1 is not a valid format").arg(filename));
    return false;
  }

  if (node["header"].IsMap()) {
    qDebug() << "File" << filename << "appears to contain model data";

    if (hasCategories) {
      CategoryData category(qPrintable(tr("New category")));
      radioData.categories.push_back(category);
    }

    radioData.models.resize(1);

    auto& model = radioData.models[modelIdx];

    try {
      if (!loadModelFromYaml(model, data)) {
        setError(tr("Cannot load ") + filename);
        return false;
      }
    } catch(const std::runtime_error& e) {
      setError(tr("Cannot load ") + filename + ":\n" + QString(e.what()));
      return false;
    }

    model.category = 0;
    model.modelIndex = modelIdx;
    strncpy(model.filename, qPrintable(filename), sizeof(model.filename) - 1);
    model.used = true;

    strncpy(radioData.generalSettings.currModelFilename, qPrintable(filename), sizeof(radioData.generalSettings.currModelFilename) - 1);
    radioData.generalSettings.currModelIndex = modelIdx;
    //  without knowing the radio this model came from the old to new radio conversion can cause more issues than it tries to solve
    //  so leave fixing incompatibilities to the user
    radioData.generalSettings.variant = getCurrentBoard();

    setWarning(tr("Please check all radio and model settings as no conversion could be performed."));
    return true;
  }
  else if (node["board"].IsScalar()) {
    setError(tr("File %1 appears to contain radio settings and importing is unsupported.").arg(filename));
  }
  else {
    setError(tr("Unable to determine content type for file %1").arg(filename));
  }

  return false;
}

bool YamlFormat::write(const RadioData & radioData)
{
  /*  Need this test in case of a new sdcard
  Board::Type board = getCurrentBoard();
  if (!HAS_EEPROM_YAML(board)) {
    qDebug() << "Board does not support YAML format";
    return false;
  }

  // ensure directories exist on sd card
  QDir dir(filename);
  dir.mkdir("RADIO");
  dir.mkdir("MODELS");
  */

  qDebug() << "Warning: format ignored - under development";
  return false; // force failure until fully developed
}
