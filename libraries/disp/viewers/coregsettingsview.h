//=============================================================================================================
/**
 * @file     coregsettingsview.h
 * @author   Ruben Dörfel <doerfelruben@aol.com>
 * @since    0.1.6
 * @date     August, 2020
 *
 * @section  LICENSE
 *
 * Copyright (C) 2020, Ruben Dörfel. All rights reserved.
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
 * @brief     CoregSettingsView class declaration.
 *
 */

#ifndef COREGSETTINGSVIEW_H
#define COREGSETTINGSVIEW_H

//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "../disp_global.h"
#include "abstractview.h"

//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QSharedPointer>

//=============================================================================================================
// EIGEN INCLUDES
//=============================================================================================================

#include <Eigen/Core>
//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================

namespace Ui {
    class CoregSettingsViewWidget;
}

namespace FIFFLIB {
    class FiffCoordTrans;
}
//=============================================================================================================
// DEFINE NAMESPACE NAMESPACE
//=============================================================================================================

namespace DISPLIB
{

//=============================================================================================================
// NAMESPACE FORWARD DECLARATIONS
//=============================================================================================================

//=============================================================================================================
/**
 * This classprovides the interface to the coregistration setting view.
 *
 * @brief Brief description of this class.
 */
class DISPSHARED_EXPORT CoregSettingsView : public AbstractView
{
    Q_OBJECT

public:
    typedef QSharedPointer<CoregSettingsView> SPtr;            /**< Shared pointer type for DISPLIB. */
    typedef QSharedPointer<const CoregSettingsView> ConstSPtr; /**< Const shared pointer type for DISPLIB. */

    //=========================================================================================================
    /**
    * Constructs a CoregSettingsView object.
    */
    explicit CoregSettingsView(const QString& sSettingsPath = "",
                             QWidget *parent = 0,
                             Qt::WindowFlags f = Qt::Widget);

    ~CoregSettingsView();

    //=========================================================================================================
    /**
     * Saves all important settings of this view via QSettings.
     */
    void saveSettings();

    //=========================================================================================================
    /**
     * Loads and inits all important settings of this view via QSettings.
     */
    void loadSettings();

    //=========================================================================================================
    /**
     * Update the views GUI based on the set GuiMode (Clinical=0, Research=1).
     *
     * @param mode     The new mode (Clinical=0, Research=1).
     */
    void updateGuiMode(GuiMode mode);

    //=========================================================================================================
    /**
     * Update the views GUI based on the set ProcessingMode (RealTime=0, Offline=1).
     *
     * @param mode     The new mode (RealTime=0, Offline=1).
     */
    void updateProcessingMode(ProcessingMode mode);

    void clearSelectionBem();

    void addSelectionBem(const QString& sBemName);

    QString getCurrentSelectedBem();

    //=========================================================================================================
    /**
     * Get the maximum number of icp iterations.
     *
     * @return  The maximum nmber of iterations.
     */
    int getMaxIter();

    //=========================================================================================================
    /**
     * Get the onvergence value.
     *
     * @return  The convergence value.
     */
    float getConvergence();

    //=========================================================================================================
    /**
     * Get the auto scale state
     *
     * @return  The auto scale state.
     */
    bool getAutoScale();

    //=========================================================================================================
    /**
     * Get the the maximum distance for digitizers from the surface
     *
     * @return  The maximum distance in m.
     */
    float getOmmitDistance();

    //=========================================================================================================
    /**
     * Set the the number of omitted points
     *
     * @param[in] iN  The maximum distance in m.
     */
    void setOmittedPoints(const int iN);

    //=========================================================================================================
    /**
     * Get the weights for the digitizer types
     *
     * @return  The weight.
     */
    float getWeightLPA();
    float getWeightRPA();
    float getWeightNAS();
    float getWeightEEG();
    float getWeightHSP();
    float getWeightHPI();

    //=========================================================================================================
    /**
     * Get the types of digitizers to use for coregistration.
     *
     * @return  The list containing the digitizer types to use for coregistration.
     */
    QList<int> getDigitizerCheckState();


    //=========================================================================================================
    /**
     * Set the transformation received from the ICP  algorithm
     *
     * @param[in] vecTrans    The traslation vector.
     * @param[in] vecAngles   The rotation angle vector.
     */
    void setTransformation(const Eigen::Vector3f& vecTrans, const Eigen::Vector3f& vecAngles);
    void setTransformation(const Eigen::Matrix4f matTrans);

protected:

private:
    //=========================================================================================================
    /**
     * Load fiducial from file
     */
    void onLoadFidFile();

    //=========================================================================================================
    /**
     * Store fiducial to file.
     */
    void onStoreFidFile();

    //=========================================================================================================
    /**
     * Load digitizer from file
     */
    void onLoadDigFile();

    //=========================================================================================================
    /**
     * Discard digitizers with maximum distance to surface
     */
    void onMaxDistChanged();

    //=========================================================================================================
    /**
     * Weights changed.
     */
    void onWeigthsChanged();

    //=========================================================================================================
    /**
     * Store the transformation.
     */
    void onStoreTrans();

    Ui::CoregSettingsViewWidget*    m_pUi;                  /**< The CoregSettingsViewWidget.*/
    QString                         m_sSettingsPath;        /**< The settings path to store the GUI settings to. */

signals:
    //=========================================================================================================
    /**
     * Emit this signal whenever new fiducials were loaded.
     *
     * @param[in] sFilePath    The file path to the fiduical file.
     */
    void fidFileChanged(const QString& sFilePath);

    //=========================================================================================================
    /**
     * Emit this signal whenever the file to store the fiducials changed
     *
     * @param[in] sFilePath    The file path to the stored fiducials.
     */
    void fidStoreFileChanged(const QString& sFilePath);

    //=========================================================================================================
    /**
     * Emit this signal whenever new digitizers were loaded.
     *
     * @param[in] sFilePath    The file path to the digitizers.
     */
    void digFileChanged(const QString& sFilePath);

    //=========================================================================================================
    /**
     * Emit this signal whenever fiducial alignment is rewuested.
     *
     */
    void fitFiducials();

    //=========================================================================================================
    /**
     * Emit this signal whenever icp alignment is requested.
     *
     */
    void fitICP();

    //=========================================================================================================
    /**
     * Emit this signal whenever icp alignment is requested.
     *
     * @param[in] sFilePath    The file path to store the transformation.
     *
     */
    void transStoreFileChanged(const QString& sFilePath);

    //=========================================================================================================
    /**
     * Emit this signal whenever the selected Bem changed.
     *
     * @param[in] sText    The file name of the currently selected Bem.
     *
     */
    void changeSelectedBem(const QString &sText);

};

//=============================================================================================================
// INLINE DEFINITIONS
//=============================================================================================================


} // NAMESPACE

#endif // COREGSETTINGSVIEW_H

