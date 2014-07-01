//=============================================================================================================
/**
* @file     tmsiimpedancewidget.h
* @author   Lorenz Esch <Lorenz.Esch@tu-ilmenau.de>
*           Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     June, 2014
*
* @section  LICENSE
*
* Copyright (C) 2014, Lorenz Esch, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of the Massachusetts General Hospital nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MASSACHUSETTS GENERAL HOSPITAL BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Contains the declaration of the TmsiImpedanceWidget class.
*
*/

#ifndef TMSIIMPEDANCEWIDGET_H
#define TMSIIMPEDANCEWIDGET_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include <utils/asaelc.h>
#include "../tmsielectrodeitem.h"

#include <xMeas/newrealtimemultisamplearray.h>

//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QWidget>
#include <QGraphicsScene>
#include <QtSvg/QSvgGenerator>

namespace Ui {
class TMSIImpedanceWidget;
}


//*************************************************************************************************************
//=============================================================================================================
// DEFINE NAMESPACE TMSIPlugin
//=============================================================================================================

namespace TMSIPlugin
{


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace Eigen;
using namespace UTILSLIB;


//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================

class TMSI;


class TMSIImpedanceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TMSIImpedanceWidget(TMSI* p_pTMSI, QWidget *parent = 0);
    ~TMSIImpedanceWidget();

    //=========================================================================================================
    /**
    * Updates the values of the electrodes placed in the QGraphicsScene.
    */
    void updateGraphicScene(VectorXd matValue);

    //=========================================================================================================
    /**
    * Initialises the 2D positions of the electrodes in the QGraphicsScene.
    */
    void initGraphicScene();

private:
    TMSI*                       m_pTMSI;                    /**< The pointer back to the TMSI plugin.*/

    QGraphicsScene              m_scene;                    /**< The QGraphicScene.*/

    QMap< QString, int >        m_qmElectrodeNameIndex;     /**< Lookup table for electrode name and their corresponding index in the received data matrix.*/

    Ui::TMSIImpedanceWidget*    ui;                         /**< The user interface for the TMSIImpedanceWidget.*/

    //=========================================================================================================
    /**
    * Adds an electrode item to the QGraphicScene.
    */
    void addElectrodeItem(QString electrodeName, QVector2D position, QColor color);

    //=========================================================================================================
    /**
    * Start the measurement process.
    */
    void startImpedanceMeasurement();

    //=========================================================================================================
    /**
    * Stops the measurement process.
    */
    void stopImpedanceMeasurement();

    //=========================================================================================================
    /**
    * Takes a screenshot of the current view.
    */
    void takeScreenshot();

    //=========================================================================================================
    /**
    * Loads a layout from file.
    */
    void loadLayout();

    //=========================================================================================================
    /**
    * Updates position of all electrodes in the scene.
    */
    void updateElectrodePositions();

    //=========================================================================================================
    /**
    * Reimplemnted closing event handler. Used to stop the measurement when closing the widget.
    */
    void closeEvent(QCloseEvent *event);

};

} // NAMESPACE

#endif // TMSIIMPEDANCEWIDGET_H
