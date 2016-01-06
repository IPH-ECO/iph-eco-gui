#ifndef TIME_SERIES_CHART_2D_INTERACTOR_STYLE_H
#define TIME_SERIES_CHART_2D_INTERACTOR_STYLE_H

#include <vtkSmartPointer.h>
#include <vtkInteractorStyleTrackballCamera.h>

class TimeSeriesChart2DInteractorStyle : public vtkInteractorStyleTrackballCamera {
public:
	static TimeSeriesChart2DInteractorStyle* New();
    vtkTypeMacro(TimeSeriesChart2DInteractorStyle, vtkInteractorStyleTrackballCamera);

    TimeSeriesChart2DInteractorStyle() {}
    
    virtual void Rotate() {}
    
    virtual void Spin() {}
    
    virtual void Dolly() {}
};

#endif // TIME_SERIES_CHART_2D_INTERACTOR_STYLE_H