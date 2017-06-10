//=============================================================================================================
/**
* @file     sensordatatreeitem.cpp
* @author   Felix Griesau <felix.griesau@tu-ilmenau.de>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     Month, Year
*
* @section  LICENSE
*
* Copyright (C) Year, Felix Griesau and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    sensordatatreeitem class definition.
*
*/


//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "sensordatatreeitem.h"
#include "../../workers/rtSourceLoc/rtsourcelocdataworker.h"
#include "../common/metatreeitem.h"

#include <mne/mne_sourceestimate.h>
#include <mne/mne_forwardsolution.h>
#include <mne/mne_bem_surface.h>
#include <fiff/fiff_evoked.h>
#include <geometryInfo/geometryinfo.h>
#include <interpolation/interpolation.h>


//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================


//*************************************************************************************************************
//=============================================================================================================
// Eigen INCLUDES
//=============================================================================================================

#include <Eigen/Core>

//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace Eigen;
using namespace MNELIB;
using namespace FIFFLIB;
using namespace DISP3DLIB;
using namespace GEOMETRYINFO;
using namespace INTERPOLATION;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE GLOBAL METHODS
//=============================================================================================================


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

SensorDataTreeItem::SensorDataTreeItem(int iType, const QString &text)
: AbstractTreeItem(iType,text)
, m_bIsDataInit(false)
{
    initItem();
}

//*************************************************************************************************************

SensorDataTreeItem::~SensorDataTreeItem()
{
    if(m_pSensorRtDataWorker->isRunning()) {
        m_pSensorRtDataWorker->stop();
        delete m_pSensorRtDataWorker;
    }
}


//*************************************************************************************************************

void SensorDataTreeItem::initItem()
{
    this->setEditable(false);
    this->setToolTip("MNE Sensordata item");

    //Add items
    QList<QStandardItem*> list;
    QVariant data;

    MetaTreeItem* pItemStreamStatus = new MetaTreeItem(MetaTreeItemTypes::StreamStatus, "Stream data on/off");
    connect(pItemStreamStatus, &MetaTreeItem::checkStateChanged,
            this, &SensorDataTreeItem::onCheckStateWorkerChanged);
    list << pItemStreamStatus;
    list << new QStandardItem(pItemStreamStatus->toolTip());
    this->appendRow(list);
    pItemStreamStatus->setCheckable(true);
    pItemStreamStatus->setCheckState(Qt::Unchecked);

    data.setValue(false);
    pItemStreamStatus->setData(data, MetaTreeItemRoles::StreamStatus);

    MetaTreeItem* pItemColormapType = new MetaTreeItem(MetaTreeItemTypes::ColormapType, "Hot");
    connect(pItemColormapType, &MetaTreeItem::dataChanged,
            this, &SensorDataTreeItem::onColormapTypeChanged);
    list.clear();
    list << pItemColormapType;
    list << new QStandardItem(pItemColormapType->toolTip());
    this->appendRow(list);
    data.setValue(QString("Hot"));
    pItemColormapType->setData(data, MetaTreeItemRoles::ColormapType);

    MetaTreeItem* pItemSourceLocNormValue = new MetaTreeItem(MetaTreeItemTypes::DistributedSourceLocThreshold, "0.0,5.5,15");
    connect(pItemSourceLocNormValue, &MetaTreeItem::dataChanged,
            this, &SensorDataTreeItem::onDataNormalizationValueChanged);
    list.clear();
    list << pItemSourceLocNormValue;
    list << new QStandardItem(pItemSourceLocNormValue->toolTip());
    this->appendRow(list);
    data.setValue(QVector3D(0.0,5.5,15));
    pItemSourceLocNormValue->setData(data, MetaTreeItemRoles::DistributedSourceLocThreshold);

    MetaTreeItem *pItemStreamingInterval = new MetaTreeItem(MetaTreeItemTypes::StreamingTimeInterval, "50");
    connect(pItemStreamingInterval, &MetaTreeItem::dataChanged,
            this, &SensorDataTreeItem::onTimeIntervalChanged);
    list.clear();
    list << pItemStreamingInterval;
    list << new QStandardItem(pItemStreamingInterval->toolTip());
    this->appendRow(list);
    data.setValue(50);
    pItemStreamingInterval->setData(data, MetaTreeItemRoles::StreamingTimeInterval);

    MetaTreeItem *pItemLoopedStreaming = new MetaTreeItem(MetaTreeItemTypes::LoopedStreaming, "Looping on/off");
    connect(pItemLoopedStreaming, &MetaTreeItem::checkStateChanged,
            this, &SensorDataTreeItem::onCheckStateLoopedStateChanged);
    pItemLoopedStreaming->setCheckable(true);
    pItemLoopedStreaming->setCheckState(Qt::Checked);
    list.clear();
    list << pItemLoopedStreaming;
    list << new QStandardItem(pItemLoopedStreaming->toolTip());
    this->appendRow(list);

    MetaTreeItem *pItemAveragedStreaming = new MetaTreeItem(MetaTreeItemTypes::NumberAverages, "1");
    connect(pItemAveragedStreaming, &MetaTreeItem::dataChanged,
            this, &SensorDataTreeItem::onNumberAveragesChanged);
    list.clear();
    list << pItemAveragedStreaming;
    list << new QStandardItem(pItemAveragedStreaming->toolTip());
    this->appendRow(list);
    data.setValue(1);
    pItemAveragedStreaming->setData(data, MetaTreeItemRoles::NumberAverages);
}


//*************************************************************************************************************

void SensorDataTreeItem::init(const MNEBemSurface &inSurface, const FiffEvoked &evoked, const qint32 sensorType)
{

        //Source Space IS clustered
       //@todo wich item do we use here ????????????????????????
       this->setData(0, Data3DTreeModelItemRoles::RTStartIdxLeftHemi);


    //set rt data corresponding to the hemisphere
    if(!m_pSensorRtDataWorker) {
        m_pSensorRtDataWorker = new RtSourceLocDataWorker();
    }


    // positions of EEG and MEG sensors
    QVector<Vector3f> sensors;
    //fill QVectors with the right sensor positions
    for( const FiffChInfo &info : evoked.info.chs)
    {
        if(info.kind == sensorType)
        {
            sensors.push_back(info.chpos.r0);
        }
    }

    std::cout << "Number of vertices: ";
    std::cout << inSurface.rr.rows() << std::endl;

    //sensor projecting
    QSharedPointer<QVector<qint32>> mappedSubSet = GeometryInfo::projectSensor(inSurface, sensors);

    //SCDC with cancel distance 0.03m
    QSharedPointer<MatrixXd> distanceMatrix = GeometryInfo::scdc(inSurface, *mappedSubSet, 0.03);
    //@todo missing filtering of bad channles

    // linear weight matrix
    Interpolation::createInterpolationMat(*mappedSubSet, distanceMatrix);

    //@todo adapt to new worker wich is to be implemented
//    connect(m_pSensorRtDataWorker.data(), &RtSourceLocDataWorker::newRtData,
//            this, &SensorDataTreeItem::onNewRtData);

    m_bIsDataInit = true;
}


//*************************************************************************************************************

void SensorDataTreeItem::addData(const MatrixXd& tSensorData)
{
    if(!m_bIsDataInit) {
        qDebug() << "SensorDataTreeItem::addData - sensor data item has not been initialized yet!";
        return;
    }

    //Set new data into item's data.
    QVariant data;
    data.setValue(tSensorData);
    //@todo RTData correct ??
    this->setData(data, Data3DTreeModelItemRoles::RTData);

    if(m_pSensorRtDataWorker) {
         m_pSensorRtDataWorker->addData(tSensorData);
    }
}


//*************************************************************************************************************

void SensorDataTreeItem::setLoopState(bool state)
{
    QList<QStandardItem*> lItems = this->findChildren(MetaTreeItemTypes::LoopedStreaming);

    for(int i = 0; i < lItems.size(); i++) {
        if(MetaTreeItem* pAbstractItem = dynamic_cast<MetaTreeItem*>(lItems.at(i))) {
            pAbstractItem->setCheckState(state == true ? Qt::Checked : Qt::Unchecked);
            QVariant data;
            data.setValue(state);
            pAbstractItem->setData(data, MetaTreeItemRoles::LoopedStreaming);
        }
    }
}


//*************************************************************************************************************

void SensorDataTreeItem::setStreamingActive(bool state)
{
    QList<QStandardItem*> lItems = this->findChildren(MetaTreeItemTypes::StreamStatus);

    for(int i = 0; i < lItems.size(); i++) {
        if(MetaTreeItem* pAbstractItem = dynamic_cast<MetaTreeItem*>(lItems.at(i))) {
            pAbstractItem->setCheckState(state == true ? Qt::Checked : Qt::Unchecked);
            QVariant data;
            data.setValue(state);
            pAbstractItem->setData(data, MetaTreeItemRoles::StreamStatus);
        }
    }
}


//*************************************************************************************************************

void SensorDataTreeItem::setTimeInterval(int iMSec)
{
    QList<QStandardItem*> lItems = this->findChildren(MetaTreeItemTypes::StreamingTimeInterval);

    for(int i = 0; i < lItems.size(); i++) {
        if(MetaTreeItem* pAbstractItem = dynamic_cast<MetaTreeItem*>(lItems.at(i))) {
            QVariant data;
            data.setValue(iMSec);
            pAbstractItem->setData(data, MetaTreeItemRoles::StreamingTimeInterval);
            pAbstractItem->setData(data, Qt::DisplayRole);
        }
    }
}


//*************************************************************************************************************

//@ do we need this ??
void SensorDataTreeItem::setNumberAverages(int iNumberAverages)
{
    QList<QStandardItem*> lItems = this->findChildren(MetaTreeItemTypes::NumberAverages);

    for(int i = 0; i < lItems.size(); i++) {
        if(MetaTreeItem* pAbstractItem = dynamic_cast<MetaTreeItem*>(lItems.at(i))) {
            QVariant data;
            data.setValue(iNumberAverages);
            pAbstractItem->setData(data, MetaTreeItemRoles::NumberAverages);
            pAbstractItem->setData(data, Qt::DisplayRole);
        }
    }
}


//*************************************************************************************************************

void SensorDataTreeItem::setColortable(const QString& sColortable)
{
    QList<QStandardItem*> lItems = this->findChildren(MetaTreeItemTypes::ColormapType);

    for(int i = 0; i < lItems.size(); i++) {
        if(MetaTreeItem* pAbstractItem = dynamic_cast<MetaTreeItem*>(lItems.at(i))) {
            QVariant data;
            data.setValue(sColortable);
            pAbstractItem->setData(data, MetaTreeItemRoles::ColormapType);
            pAbstractItem->setData(data, Qt::DisplayRole);
        }
    }
}


//*************************************************************************************************************

//@ if needed change signature
void SensorDataTreeItem::setColorOrigin(const MatrixX3f& matVertColorLeftHemisphere, const MatrixX3f& matVertColorRightHemisphere)
{
    m_pSensorRtDataWorker->setSurfaceColor(matVertColorLeftHemisphere,
                                             matVertColorRightHemisphere);
}


//*************************************************************************************************************

void SensorDataTreeItem::onCheckStateWorkerChanged(const Qt::CheckState& checkState)
{
    if(m_pSensorRtDataWorker) {
        if(checkState == Qt::Checked) {
            m_pSensorRtDataWorker->start();
        } else if(checkState == Qt::Unchecked) {
            m_pSensorRtDataWorker->stop();
        }
    }
}


//*************************************************************************************************************

void SensorDataTreeItem::onNewRtData(const MatrixXd &sensorData)
{
    QVariant data;
    data.setValue(sensorData);
    emit rtVertColorChanged(data);
}


//*************************************************************************************************************

void SensorDataTreeItem::onColormapTypeChanged(const QVariant& sColormapType)
{
    if(sColormapType.canConvert<QString>()) {
        if(m_pSensorRtDataWorker) {
            m_pSensorRtDataWorker->setColormapType(sColormapType.toString());
        }
    }
}


//*************************************************************************************************************

void SensorDataTreeItem::onTimeIntervalChanged(const QVariant& iMSec)
{
    if(iMSec.canConvert<int>()) {
        if(m_pSensorRtDataWorker) {
            m_pSensorRtDataWorker->setInterval(iMSec.toInt());
        }
    }
}


//*************************************************************************************************************

void SensorDataTreeItem::onDataNormalizationValueChanged(const QVariant& vecThresholds)
{
    if(vecThresholds.canConvert<QVector3D>()) {
        if(m_pSensorRtDataWorker) {
            m_pSensorRtDataWorker->setNormalization(vecThresholds.value<QVector3D>());
        }
    }
}


//*************************************************************************************************************

void SensorDataTreeItem::onCheckStateLoopedStateChanged(const Qt::CheckState& checkState)
{
    if(m_pSensorRtDataWorker) {
        if(checkState == Qt::Checked) {
            m_pSensorRtDataWorker->setLoop(true);
        } else if(checkState == Qt::Unchecked) {
            m_pSensorRtDataWorker->setLoop(false);
        }
    }
}


//*************************************************************************************************************

void SensorDataTreeItem::onNumberAveragesChanged(const QVariant& iNumAvr)
{
    if(iNumAvr.canConvert<int>()) {
        if(m_pSensorRtDataWorker) {
            m_pSensorRtDataWorker->setNumberAverages(iNumAvr.toInt());
        }
    }
}

