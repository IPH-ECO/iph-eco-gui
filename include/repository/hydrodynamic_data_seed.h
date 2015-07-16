#ifndef HYDRODYNAMIC_DATA_SEED_H
#define HYDRODYNAMIC_DATA_SEED_H

#include "include/domain/hydrodynamic_process.h"
#include <QList>
#include <limits>

class HydrodynamicDataSeed {
private:
    QList<HydrodynamicProcess*> processes;
    QList<HydrodynamicParameter*> parameters;
    
    template<typename T>
    T* findByName(const QList<T*> &list, const QString &name) const {
        for (int i = 0; i < list.size(); i++) {
            T *element = list[i];
            
            if (element->getName() == name) {
                return element;
            }
        }
        
        return nullptr;
    }
public:
    HydrodynamicDataSeed() {
        // Parameters
        HydrodynamicParameter *paConstants = new HydrodynamicParameter("constants", "Constants", nullptr, false);
        HydrodynamicParameter *paGravityAcceleration = new HydrodynamicParameter("gravityAcceleration", "Gravity acceleration (m/s\u00b2)", paConstants);
        HydrodynamicParameter *paWaterDensity = new HydrodynamicParameter("waterDensity", "Water density of reference (kg/m\u00b3)", paConstants);
        HydrodynamicParameter *paWaterTemperature = new HydrodynamicParameter("waterTemperature", "Water temperature of reference (\u00b0C)", paConstants);
        HydrodynamicParameter *paAirTemperature = new HydrodynamicParameter("airTemperature", "Air temperature of reference (\u00b0C)", paConstants);

        parameters << paConstants << paGravityAcceleration << paWaterDensity << paWaterTemperature << paAirTemperature;
        
        HydrodynamicParameter *paNumericalParameters = new HydrodynamicParameter("numericalParameters", "Numerical Parameters", nullptr, false);
        HydrodynamicParameter *paThetaCoefficient = new HydrodynamicParameter("numericalParameters", "Theta coefficient (-)", paNumericalParameters);
        HydrodynamicParameter *paThresholdDepth = new HydrodynamicParameter("numericalParameters", "Threshold depth for Wet and Dry Algorithm", paNumericalParameters);
        HydrodynamicParameter *paSmoothingTime = new HydrodynamicParameter("numericalParameters", "Smoothing time", paNumericalParameters);

        parameters << paNumericalParameters << paThetaCoefficient << paThresholdDepth << paSmoothingTime;

        HydrodynamicParameter *paWindStress = new HydrodynamicParameter("windStress", "Wind stress on water surface", nullptr, false);
        HydrodynamicParameter *paAirDensity = new HydrodynamicParameter("airDensity", "Air density (kg/m\u00b3)", paWindStress);
        HydrodynamicParameter *paWindDrag = new HydrodynamicParameter("windDrag", "Wind drag coefficient", paWindStress, false);
        HydrodynamicParameter *paWindDragConstant = new HydrodynamicParameter("windDragConstant", "Constant", paWindDrag);
        HydrodynamicParameter *paWindDragLinearFunction = new HydrodynamicParameter("windDragLinearFunction", "Linear function", paWindDrag, false);
        HydrodynamicParameter *paWindDragBreakpointA = new HydrodynamicParameter("windDragBreakpointA", "Breakpoint A", paWindDragLinearFunction, false);
        HydrodynamicParameter *paWindDragC1 = new HydrodynamicParameter("windDragCoefficientC1", "Coefficient c1", paWindDragBreakpointA);
        HydrodynamicParameter *paWindDragW1 = new HydrodynamicParameter("windDragWindSpeedW1", "Wind Speed w1", paWindDragBreakpointA);
        HydrodynamicParameter *paWindDragBreakpointB = new HydrodynamicParameter("windDragBreakpointB", "Breakpoint B", paWindDragLinearFunction, false);
        HydrodynamicParameter *paWindDragC2 = new HydrodynamicParameter("windDragCoefficientC2", "Coefficient c2", paWindDragBreakpointB);
        HydrodynamicParameter *paWindDragW2 = new HydrodynamicParameter("windDragWindSpeedW2", "Wind Speed w2", paWindDragBreakpointB);
        HydrodynamicParameter *paWindDragBreakpointC = new HydrodynamicParameter("windDragBreakpointC", "Breakpoint C", paWindDragLinearFunction, false);
        HydrodynamicParameter *paWindDragC3 = new HydrodynamicParameter("windDragCoefficientC3", "Coefficient c3", paWindDragBreakpointC);
        HydrodynamicParameter *paWindDragW3 = new HydrodynamicParameter("windDragWindSpeedW3", "Wind Speed w3", paWindDragBreakpointC);

        parameters << paWindStress << paAirDensity << paWindDrag << paWindDragConstant << paWindDragLinearFunction;
        parameters << paWindDragBreakpointA << paWindDragC1 << paWindDragW1;
        parameters << paWindDragBreakpointB << paWindDragC2 << paWindDragW2;
        parameters << paWindDragBreakpointC << paWindDragC3 << paWindDragW3;

        HydrodynamicParameter *paBottomRoughness = new HydrodynamicParameter("bottomRoughness", "Bottom roughness", nullptr, false);
        HydrodynamicParameter *paChezyConstant = new HydrodynamicParameter("roughnessChezyConstant", "Chezy constant", paBottomRoughness);
        HydrodynamicParameter *paManningConstant = new HydrodynamicParameter("roughnessManningConstant", "Manning constant", paBottomRoughness);
        HydrodynamicParameter *paWhiteColebrookConstant = new HydrodynamicParameter("roughnessWhiteColebrookConstant", "White-Colebrook constant", paBottomRoughness);
        HydrodynamicParameter *paChezyGridData = new HydrodynamicParameter("roughnessChezyUseGridData", "Use values defined at Grid Data for Chezy", paBottomRoughness, false);
        HydrodynamicParameter *paManningGridData = new HydrodynamicParameter("roughnessManningConstant", "Use values defined at Grid Data for Manning", paBottomRoughness, false);
        HydrodynamicParameter *paWhiteColebrookGridData = new HydrodynamicParameter("roughnessWhiteColebrookUseGridData", "Use values defined at Grid Data for White-Colebrook", paBottomRoughness, false);

        parameters << paBottomRoughness << paChezyConstant << paManningConstant << paWhiteColebrookConstant;
        parameters << paChezyGridData << paManningGridData << paWhiteColebrookGridData;

        HydrodynamicParameter *paViscosityDiffusivity = new HydrodynamicParameter("viscosityDiffusivityFormulation", "Viscosity/Diffusivity (Turbulence Model)", nullptr, false);
        HydrodynamicParameter *paHorizontalEddyVD = new HydrodynamicParameter("horizontalEddyVD", "Horizontal eddy viscosity/diffusivity", paViscosityDiffusivity, false);
        HydrodynamicParameter *paHorizontalEddyConstant = new HydrodynamicParameter("horizontalEddyConstant", "Constant", paHorizontalEddyVD, false);
        HydrodynamicParameter *paHorizontalEddyViscosity = new HydrodynamicParameter("horizontalEddyViscosity", "Horizontal eddy viscosity", paHorizontalEddyConstant);
        HydrodynamicParameter *paHorizontalEddyDiffusivity = new HydrodynamicParameter("horizontalEddyDiffusivity", "Horizontal eddy diffusivity", paHorizontalEddyConstant);
        HydrodynamicParameter *paSmagorinskyModel = new HydrodynamicParameter("smagorinskyModel", "Smagorinsky model", paHorizontalEddyVD, false);
        HydrodynamicParameter *paSmagorinskyCoefficient = new HydrodynamicParameter("smagorinskyCoefficient", "Smagorinsky coefficient", paSmagorinskyModel);
        HydrodynamicParameter *paBackgroundHorizontalEddy = new HydrodynamicParameter("backgroundVerticalEddyDiffusivity", "Background horizontal eddy diffusivity", paSmagorinskyModel);
        HydrodynamicParameter *paVerticalEddyVD = new HydrodynamicParameter("verticalEddyVD", "Vertical eddy viscosity/diffusivity", paViscosityDiffusivity, false);
        HydrodynamicParameter *paVerticalEddyConstant = new HydrodynamicParameter("verticalEddyConstant", "Constant", paVerticalEddyVD, false);
        HydrodynamicParameter *paVerticalEddyViscosity = new HydrodynamicParameter("verticalEddyViscosity", "Vertical eddy viscosity", paVerticalEddyConstant);
        HydrodynamicParameter *paVerticalEddyDiffusivity = new HydrodynamicParameter("verticalEddyDiffusivity", "Vertical eddy diffusivity", paVerticalEddyConstant);
        HydrodynamicParameter *paZeroEquationModel = new HydrodynamicParameter("zeroEquationModel", "Zero-Equation Model", paVerticalEddyVD, false);
        HydrodynamicParameter *paRefVerticalEddyViscosity = new HydrodynamicParameter("refVerticalEddyViscosity", "Reference vertical eddy viscosity", paZeroEquationModel);
        HydrodynamicParameter *paBackgroundVerticalEddyViscosity = new HydrodynamicParameter("backgroundVerticalEddyViscosity", "Background vertical eddy viscosity", paZeroEquationModel);
        HydrodynamicParameter *paBackgroundVerticalEddyDiffusivity = new HydrodynamicParameter("backgroundVerticalEddyDiffusivity", "Background vertical eddy diffusivity", paZeroEquationModel);
        HydrodynamicParameter *paGenericLengthScaleModel = new HydrodynamicParameter("genericLengthScaleModel", "Generic Length Scale Model", paVerticalEddyVD);
        HydrodynamicParameter *paMellorYamadaModel = new HydrodynamicParameter("mellorAndYamadaTurbulenceModel", "Mellor and Yamada Turbulence Model", paVerticalEddyVD);
        HydrodynamicParameter *paElcomMixingModel = new HydrodynamicParameter("elcomMixingModel", "ELCOM Mixing Model", paVerticalEddyVD);
        HydrodynamicParameter *paTurbulenceModel = new HydrodynamicParameter("turbulenceModelInBottomBoundaryLayers", "Turbulence model in the bottom boundary layers", paViscosityDiffusivity, false, false);

        parameters << paViscosityDiffusivity << paHorizontalEddyVD << paHorizontalEddyConstant << paHorizontalEddyViscosity << paHorizontalEddyDiffusivity;
        parameters << paSmagorinskyModel << paSmagorinskyCoefficient << paBackgroundHorizontalEddy << paVerticalEddyVD << paVerticalEddyConstant;
        parameters << paVerticalEddyViscosity << paVerticalEddyDiffusivity << paZeroEquationModel << paRefVerticalEddyViscosity;
        parameters << paBackgroundVerticalEddyViscosity << paBackgroundVerticalEddyDiffusivity << paGenericLengthScaleModel << paMellorYamadaModel;
        parameters << paElcomMixingModel << paTurbulenceModel;

        HydrodynamicParameter *paAdvectionScheme = new HydrodynamicParameter("advectionScheme", "Advection Scheme for Momentum Equation", nullptr, false);
        HydrodynamicParameter *paEulerianMethod = new HydrodynamicParameter("eulerianLagrangianMethod", "Eulerian-Lagrangian Method", paAdvectionScheme, false);
        HydrodynamicParameter *paNumberOfSubTimeSteps = new HydrodynamicParameter("numberOfSubTimeSteps", "Number of sub-time steps", paEulerianMethod);
        HydrodynamicParameter *paMusclMethod = new HydrodynamicParameter("secondOrderMusclMethod", "Second Order MUSCL Method", paAdvectionScheme, false);
        HydrodynamicParameter *paEnoWenoMethod = new HydrodynamicParameter("highOrderEnoWenoMethod", "High-order ENO/WENO Method", paAdvectionScheme, false);
        HydrodynamicParameter *paNoAdvectionScheme = new HydrodynamicParameter("noAdvectionScheme", "No advection scheme", paAdvectionScheme, false);

        parameters << paAdvectionScheme << paEulerianMethod << paNumberOfSubTimeSteps << paMusclMethod << paEnoWenoMethod << paNoAdvectionScheme;

        HydrodynamicParameter *paPressure = new HydrodynamicParameter("pressure", "Pressure", nullptr, false);
        HydrodynamicParameter *paBarotropicBaroclinic = new HydrodynamicParameter("barotropicAndBaroclinicContribuitions", "Barotropic and the baroclinic contributions to the hydrostatic pressure", paPressure);
        HydrodynamicParameter *paNonHydrostaticPressure = new HydrodynamicParameter("nonHydrostaticPressure", "Non-hydrostatic pressure", paPressure, false);

        parameters << paPressure << paBarotropicBaroclinic << paNonHydrostaticPressure;
        
        HydrodynamicParameter *paCoriolis = new HydrodynamicParameter("coriolisFactor", "Coriolis factor", nullptr, false);
        HydrodynamicParameter *paCoriolisCoeffiecient = new HydrodynamicParameter("coriolisCoefficient", "Coriolis coefficient", paCoriolis);
        HydrodynamicParameter *paEarthRotationSpeed = new HydrodynamicParameter("earthsRotationSpeed", "Earth's rotation speed (rad/s)", paCoriolis);

        parameters << paCoriolis << paCoriolisCoeffiecient << paEarthRotationSpeed;

        // Processes
        HydrodynamicProcess *prWindStress = new HydrodynamicProcess("windStress", "Wind stress on water surface");
        HydrodynamicProcess *prAirDensity = new HydrodynamicProcess("airDensity", "Based on air density (kg/m\u00b3)", prWindStress, true, paAirDensity);
        HydrodynamicProcess *prWindDrag = new HydrodynamicProcess("windDrag", "Based on wind drag coefficient", prWindStress, true, paWindDrag);
        HydrodynamicProcess *prWindDragConstant = new HydrodynamicProcess("windDragConstant", "Constant", prWindDrag, true, paWindDragConstant);
        HydrodynamicProcess *prWindDragLinearFunction = new HydrodynamicProcess("windDragLinearFunction", "Linear function", prWindDrag, true, paWindDragLinearFunction);

        processes << prWindStress << prAirDensity << prWindDrag << prWindDragConstant << prWindDragLinearFunction;

        HydrodynamicProcess *prBottomRoughness = new HydrodynamicProcess("bottomRoughness", "Bottom roughness");
        HydrodynamicProcess *prRoughnessFormulation = new HydrodynamicProcess("roughnessFormulation", "Roughness formulation", prBottomRoughness);
        HydrodynamicProcess *prChezy = new HydrodynamicProcess("roughnessChezy", "Chezy (m\u207b\u00bd/s)", prRoughnessFormulation, true, paChezyConstant);
        HydrodynamicProcess *prChezyConstant = new HydrodynamicProcess("roughnessChezyConstant", "Constant", prChezy, true, paChezyConstant);
        HydrodynamicProcess *prChezyGridData = new HydrodynamicProcess("roughnessChezyUseGridData", "Use values defined at Grid Data", prChezy, true, paChezyGridData);
        HydrodynamicProcess *prManning = new HydrodynamicProcess("roughnessManning", "Manning (m\u207b\u2153/s)", prRoughnessFormulation, true, paManningConstant);
        HydrodynamicProcess *prManningConstant = new HydrodynamicProcess("roughnessManningConstant", "Constant", prManning, true, paManningConstant);
        HydrodynamicProcess *prManningGridData = new HydrodynamicProcess("roughnessManningUseGridData", "Use values defined at Grid Data", prManning, true, paManningGridData);
        HydrodynamicProcess *prWhiteColebrook = new HydrodynamicProcess("roughnessWhiteColebrook", "White-Colebrook (m)", prRoughnessFormulation, true, paWhiteColebrookConstant);
        HydrodynamicProcess *prWhiteColebrookConstant = new HydrodynamicProcess("roughnessWhiteColebrookConstant", "Constant", prWhiteColebrook, true, paWhiteColebrookConstant);
        HydrodynamicProcess *prWhiteColebrookGridData = new HydrodynamicProcess("roughnessWhiteColebrookUseGridData", "Use values defined at Grid Data", prWhiteColebrook, true, paWhiteColebrookGridData);

        processes << prBottomRoughness << prRoughnessFormulation << prChezy << prChezyConstant << prChezyGridData;
        processes << prManning << prManningConstant << prManningGridData << prWhiteColebrook << prWhiteColebrookConstant << prWhiteColebrookGridData;

        HydrodynamicProcess *prViscosityDiffusivity = new HydrodynamicProcess("viscosityDiffusivity", "Viscosity/Diffusivity (Turbulence Model)");
        HydrodynamicProcess *prHorizontalEddy = new HydrodynamicProcess("horizontalEddyViscosityDifusivity", "Horizontal eddy viscosity/diffusivity", prViscosityDiffusivity);
        HydrodynamicProcess *prHorizontalEddyConstant = new HydrodynamicProcess("horizontalEddyViscosityDifusivityConstant", "Constant", prHorizontalEddy, true, paHorizontalEddyConstant);
        HydrodynamicProcess *prSmagorinskyModel = new HydrodynamicProcess("smagorinskyModel", "Smagorinsky model", prHorizontalEddy, true, paSmagorinskyModel);
        HydrodynamicProcess *prVerticalEddy = new HydrodynamicProcess("verticalEddyViscosityDifusivity", "Vertical eddy viscosity/diffusivity", prViscosityDiffusivity);
        HydrodynamicProcess *prVerticalEddyConstant = new HydrodynamicProcess("verticalEddyViscosityDifusivityConstant", "Constant", prVerticalEddy, true, paVerticalEddyConstant);
        HydrodynamicProcess *prZeroEquationModel = new HydrodynamicProcess("zeroEquationModel", "Zero-Equation Model", prVerticalEddy, true, paZeroEquationModel);
        HydrodynamicProcess *prGenericLengthScaleModel = new HydrodynamicProcess("genericLengthScaleModel", "Generic Length Scale Model", prVerticalEddy, true, paGenericLengthScaleModel);
        HydrodynamicProcess *prMellorYamadaModel = new HydrodynamicProcess("mellorAndYamadaTurbulenceModel", "Mellor and Yamada Turbulence Model", prVerticalEddy, true, paMellorYamadaModel);
        HydrodynamicProcess *prElcomModel = new HydrodynamicProcess("elcomMixingModel", "ELCOM Mixing Model", prVerticalEddy, true, paElcomMixingModel);
        HydrodynamicProcess *prTurbulenceModel = new HydrodynamicProcess("turbulenceModelInBottomBoundaryLayers", "Turbulence model in the bottom boundary layers", prViscosityDiffusivity, true, paTurbulenceModel);

        processes << prViscosityDiffusivity << prHorizontalEddy << prHorizontalEddyConstant << prSmagorinskyModel;
        processes << prVerticalEddy << prVerticalEddyConstant << prZeroEquationModel << prGenericLengthScaleModel;
        processes << prMellorYamadaModel << prElcomModel << prTurbulenceModel;

        HydrodynamicProcess *prAdvectionScheme = new HydrodynamicProcess("advectionScheme", "Advection Scheme for Momentum Equation");
        HydrodynamicProcess *prEulerianMethod = new HydrodynamicProcess("eulerianLagrangianMethod", "Eulerian-Lagrangian Method", prAdvectionScheme, true, paEulerianMethod);
        HydrodynamicProcess *prMusclMethod = new HydrodynamicProcess("secondOrderMusclMethod", "Second Order MUSCL Method", prAdvectionScheme, true, paMusclMethod);
        HydrodynamicProcess *prEnoWenoMethod = new HydrodynamicProcess("highOrderEnoWenoMethod", "High-order ENO/WENO Method", prAdvectionScheme, true, paEnoWenoMethod);
        HydrodynamicProcess *prNoAdvectionScheme = new HydrodynamicProcess("noAdvectionScheme", "No advection scheme", prAdvectionScheme, true, paNoAdvectionScheme);
        
        processes << prAdvectionScheme << prEulerianMethod << prMusclMethod << prEnoWenoMethod << prNoAdvectionScheme;

        // Pressure
        HydrodynamicProcess *prPressure = new HydrodynamicProcess("pressure", "Pressure");
        HydrodynamicProcess *prBarotropicBaroclinic = new HydrodynamicProcess("barotropicAndBaroclinicContribuitions", "Barotropic and the baroclinic contributions to the hydrostatic pressure", prPressure, true, paBarotropicBaroclinic);
        HydrodynamicProcess *prNonHydrostaticPressure = new HydrodynamicProcess("nonHydrostaticPressure", "Non-hydrostatic pressure", prPressure, true, paNonHydrostaticPressure);

        processes << prPressure << prBarotropicBaroclinic << prNonHydrostaticPressure;
        
        // Coriolis
        HydrodynamicProcess *prCoriolis = new HydrodynamicProcess("coriolisFactor", "Coriolis factor", nullptr, true, paCoriolis);

        processes << prCoriolis;

        // Default processes
        prWindDragConstant->setChecked(true);
        prChezyConstant->setChecked(true);
        prHorizontalEddyConstant->setChecked(true);
        prVerticalEddyConstant->setChecked(true);
        prEulerianMethod->setChecked(true);

        // Default parameter values and ranges
        paGravityAcceleration->setDefaultValue(9.81);
        paGravityAcceleration->setRangeMinimum(9.78);
        paGravityAcceleration->setRangeMaximum(9.83);
        paWaterDensity->setDefaultValue(1000);
        paWaterTemperature->setDefaultValue(20);
        paWaterTemperature->setRangeMinimum(-100);
        paWaterTemperature->setRangeMaximum(100);
        paAirTemperature->setDefaultValue(20);
        paAirTemperature->setRangeMinimum(-100);
        paAirTemperature->setRangeMaximum(100);
        paThetaCoefficient->setDefaultValue(0.55);
        paThetaCoefficient->setRangeMinimum(0);
        paThetaCoefficient->setRangeMaximum(1);
        paThresholdDepth->setDefaultValue(0.05);
        paThresholdDepth->setRangeMinimum(0);
        paThresholdDepth->setRangeMaximum(std::numeric_limits<double>::max());
        paWindDragConstant->setDefaultValue(1e-7);
        paWindDragConstant->setRangeMinimum(1e-10);
        paWindDragConstant->setRangeMaximum(1e-3);
        paHorizontalEddyViscosity->setDefaultValue(1);
        paHorizontalEddyViscosity->setRangeMinimum(1e-10);
        paHorizontalEddyViscosity->setRangeMaximum(1000);
        paHorizontalEddyDiffusivity->setDefaultValue(1);
        paHorizontalEddyDiffusivity->setRangeMinimum(1e-10);
        paHorizontalEddyDiffusivity->setRangeMaximum(1000);
        paVerticalEddyViscosity->setDefaultValue(1);
        paVerticalEddyViscosity->setRangeMinimum(1e-10);
        paVerticalEddyViscosity->setRangeMaximum(1000);
        paVerticalEddyDiffusivity->setDefaultValue(1);
        paVerticalEddyDiffusivity->setRangeMinimum(1e-10);
        paVerticalEddyDiffusivity->setRangeMaximum(1000);
        paSmagorinskyCoefficient->setDefaultValue(0.15);
        paSmagorinskyCoefficient->setRangeMinimum(0.1);
        paSmagorinskyCoefficient->setRangeMaximum(0.5);
        paBackgroundHorizontalEddy->setDefaultValue(1e-5);
        paBackgroundHorizontalEddy->setRangeMinimum(1e-10);
        paBackgroundHorizontalEddy->setRangeMaximum(1e-1);
        paRefVerticalEddyViscosity->setDefaultValue(1e-2);
        paRefVerticalEddyViscosity->setRangeMinimum(1e-10);
        paRefVerticalEddyViscosity->setRangeMaximum(1);
        paBackgroundVerticalEddyViscosity->setDefaultValue(1e-4);
        paBackgroundVerticalEddyViscosity->setRangeMinimum(1e-10);
        paBackgroundVerticalEddyViscosity->setRangeMaximum(1);
        paBackgroundVerticalEddyDiffusivity->setDefaultValue(1e-5);
        paBackgroundVerticalEddyDiffusivity->setRangeMinimum(1e-10);
        paBackgroundVerticalEddyDiffusivity->setRangeMaximum(1);
        paNumberOfSubTimeSteps->setDefaultValue(10);
        paNumberOfSubTimeSteps->setRangeMinimum(1);
        paNumberOfSubTimeSteps->setRangeMaximum(1000);
        paEarthRotationSpeed->setDefaultValue(7.2921e-5);
        paEarthRotationSpeed->setRangeMinimum(0);
        paEarthRotationSpeed->setRangeMaximum(1);
    }

    QList<HydrodynamicParameter*> getParameters() {
        return parameters;
    }

    QList<HydrodynamicProcess*> getProcesses() {
        return processes;
    }
    
    HydrodynamicProcess* findProcessByName(const QString &name) const {
        return findByName<HydrodynamicProcess>(processes, name);
    }

    HydrodynamicParameter* findParameterNodeByName(const QString &name) const {
        return findByName<HydrodynamicParameter>(parameters, name);
    }
};

#endif // HYDRODYNAMIC_DATA_SEED_H
