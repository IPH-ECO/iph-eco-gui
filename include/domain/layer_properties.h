#ifndef LAYER_PROPERTIES_H
#define LAYER_PROPERTIES_H

#include "color_gradient.h"
#include <QString>

enum class VectorColorMode { MAGNITUDE = 1, CONSTANT };

class LayerProperties {
private:
    bool useCustomMapMinimum;
    bool useCustomMapMaximum;
    double customMapMinimumRange;
    double customMapMaximumRange;
    QString mapColorGradient;
    bool mapInvertColorGradient;
    int mapOpacity;
    bool mapLegend;
    bool mapLighting;
    
    bool useCustomPointsMinimum;
    bool useCustomPointsMaximum;
    double customPointsMinimumRange;
    double customPointsMaximumRange;
    QString pointsColorGradient;
    bool pointsInvertColorGradient;
    int pointsOpacity;
    int pointsSize;
    bool pointsLegend;
    
    bool useCustomVectorsMinimum;
    bool useCustomVectorsMaximum;
    double customVectorsMinimumRange;
    double customVectorsMaximumRange;
    VectorColorMode vectorColorMode;
    QString vectorsColorGradient;
    QString vectorsColor;
    bool vectorsInvertColorGradient;
    int vectorsOpacity;
    double vectorsScale;
    int vectorsWidth;
    bool vectorsLegend;
    
    QString meshLineColor;
    int meshLineStyle;
    int meshLineWidth;
    int meshOpacity;
public:
	LayerProperties();

	bool getUseCustomMapMinimum() const;
    void setUseCustomMapMinimum(const bool &useCustomMapMinimum);
    bool getUseCustomMapMaximum() const;
    void setUseCustomMapMaximum(const bool &useCustomMapMaximum);
    double getCustomMapMininumRange() const;
    void setCustomMapMinimumRange(const double &customMapMinimumRange);
    double getCustomMapMaximumRange() const;
    void setCustomMapMaximumRange(const double &customMapMaximumRange);
    QString getMapColorGradient() const;
    void setMapColorGradient(const QString &mapColorGradient);
    bool getMapInvertColorGradient() const;
    void setMapInvertColorGradient(const bool &mapInvertColorGradient);
    int getMapOpacity() const;
    void setMapOpacity(const int &mapOpacity);
    bool getMapLegend() const;
    void setMapLegend(const bool &mapLegend);
    bool getMapLighting() const;
    void setMapLighting(const bool &mapLighting);
    
    bool getUseCustomPointsMinimum() const;
    void setUseCustomPointsMinimum(const bool &useCustomPointsMinimum);
    bool getUseCustomPointsMaximum() const;
    void setUseCustomPointsMaximum(const bool &useCustomPointsMaximum);
    double getCustomPointsMininumRange() const;
    void setCustomPointsMinimumRange(const double &customPointsMinimumRange);
    double getCustomPointsMaximumRange() const;
    void setCustomPointsMaximumRange(const double &customPointsMaximumRange);
    QString getPointsColorGradient() const;
    void setPointsColorGradient(const QString &pointsColorGradient);
    bool getPointsInvertColorGradient() const;
    void setPointsInvertColorGradient(const bool &pointsInvertColorGradient);
    int getPointsOpacity() const;
    void setPointsOpacity(const int &pointsOpacity);
    int getPointsSize() const;
    void setPointsSize(const int &pointsSize);
    bool getPointsLegend() const;
    void setPointsLegend(const bool &pointsLegend);
    
    bool getUseCustomVectorsMinimum() const;
    void setUseCustomVectorsMinimum(const bool &useCustomVectorsMinimum);
    bool getUseCustomVectorsMaximum() const;
    void setUseCustomVectorsMaximum(const bool &useCustomVectorsMaximum);
    double getCustomVectorsMinimumRange() const;
    void setCustomVectorsMinimumRange(const double &customVectorsMinimumRange);
    double getCustomVectorsMaximumRange() const;
    void setCustomVectorsMaximumRange(const double &customVectorsMaximumRange);
    VectorColorMode getVectorColorMode() const;
    void setVectorColorMode(const VectorColorMode &vectorColorMode);
    QString getVectorsColorGradient() const;
    void setVectorsColorGradient(const QString &vectorsColorGradient);
    QString getVectorsColor() const;
    void setVectorsColor(const QString &vectorsColor);
    bool getVectorsInvertColorGradient() const;
    void setVectorsInvertColorGradient(const bool &vectorsInvertColorGradient);
    int getVectorsOpacity() const;
    void setVectorsOpacity(const int &vectorsOpacity);
    double getVectorsScale() const;
    void setVectorsScale(const double &vectorsScale);
    int getVectorsWidth() const;
    void setVectorsWidth(const int &vectorsWidth);
    bool getVectorsLegend() const;
    void setVectorsLegend(const bool &vectorsLegend);
    
    QString getMeshLineColor() const;
    void setMeshLineColor(const QString &meshLineColor);
    int getMeshLineStyle() const;
    void setMeshLineStyle(const int &meshLineStyle);
    int getMeshLineWidth() const;
    void setMeshLineWidth(const int &meshLineWidth);
    int getMeshOpacity() const;
    void setMeshOpacity(const int &meshOpacity);
};

#endif // LAYER_PROPERTIES_H