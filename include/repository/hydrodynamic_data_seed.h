#ifndef HYDRODYNAMIC_DATA_SEED_H
#define HYDRODYNAMIC_DATA_SEED_H

#include "include/domain/hydrodynamic_process.h"
#include <QList>

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
        HydrodynamicParameter *paWindDrag = new HydrodynamicParameter("windDrag", "Wind drag coefficient", paWindStress);
        HydrodynamicParameter *paWindDragConstant = new HydrodynamicParameter("windDragConstant", "Constant", paWindDrag);
        HydrodynamicParameter *paWindDragLinearFunction = new HydrodynamicParameter("windDragLinearFunction", "Linear function", paWindDrag);
        HydrodynamicParameter *paWindDragBreakpointA = new HydrodynamicParameter("windDragBreakpointA", "Breakpoint A", paWindDragLinearFunction);
        HydrodynamicParameter *paWindDragC1 = new HydrodynamicParameter("windDragCoefficientC1", "Coefficient c1", paWindDragBreakpointA);
        HydrodynamicParameter *paWindDragW1 = new HydrodynamicParameter("windDragWindSpeedW1", "Wind Speed w1", paWindDragBreakpointA);
        HydrodynamicParameter *paWindDragBreakpointB = new HydrodynamicParameter("windDragBreakpointB", "Breakpoint B", paWindDragLinearFunction);
        HydrodynamicParameter *paWindDragC2 = new HydrodynamicParameter("windDragCoefficientC2", "Coefficient c2", paWindDragBreakpointB);
        HydrodynamicParameter *paWindDragW2 = new HydrodynamicParameter("windDragWindSpeedW2", "Wind Speed w2", paWindDragBreakpointB);
        HydrodynamicParameter *paWindDragBreakpointC = new HydrodynamicParameter("windDragBreakpointC", "Breakpoint C", paWindDragLinearFunction);
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

        HydrodynamicParameter *paViscosityDiffusivity = new HydrodynamicParameter("viscosityDiffusivityFormulation", "Viscosity/Diffusivity (Turbulence Model)");
        HydrodynamicParameter *paHorizontalEddy = new HydrodynamicParameter("horizontalEddyViscosityDifusivity", "Horizontal eddy viscosity/diffusivity", paViscosityDiffusivity, false);
        HydrodynamicParameter *paHorizontalEddyConstant = new HydrodynamicParameter("horizontalEddyViscosityDifusivityConstant", "Constant", paHorizontalEddy);
        HydrodynamicParameter *paSmagorinskyModel = new HydrodynamicParameter("smagorinskyModel", "Smagorinsky model", paHorizontalEddy);
        HydrodynamicParameter *paSmagorinskyCoefficient = new HydrodynamicParameter("smagorinskyCoefficient", "Smagorinsky coefficient", paSmagorinskyModel);
        HydrodynamicParameter *paBackgroundHorizontalEddy = new HydrodynamicParameter("backgroundVerticalEddyDiffusivity", "Background horizontal eddy diffusivity", paSmagorinskyModel);
        HydrodynamicParameter *paVerticalEddy = new HydrodynamicParameter("verticalEddyViscosityDifusivity", "Vertical eddy viscosity/diffusivity", paViscosityDiffusivity);
        HydrodynamicParameter *paVerticalEddyConstant = new HydrodynamicParameter("verticalEddyViscosityDifusivityConstant", "Constant", paVerticalEddy);
        HydrodynamicParameter *paZeroEquationModel = new HydrodynamicParameter("zeroEquationModel", "Zero-Equation Model", paVerticalEddy, false);
        HydrodynamicParameter *paVerticalEddyViscosity = new HydrodynamicParameter("referenceVerticalEddyViscosity", "Reference vertical eddy viscosity", paZeroEquationModel);
        HydrodynamicParameter *paBackgroundVerticalEddyViscosity = new HydrodynamicParameter("backgroundVerticalEddyViscosity", "Background vertical eddy viscosity", paZeroEquationModel);
        HydrodynamicParameter *paBackgroundVerticalEddyDiffusivity = new HydrodynamicParameter("backgroundVerticalEddyDiffusivity", "Background vertical eddy diffusivity", paZeroEquationModel);
        HydrodynamicParameter *paGenericLengthScaleModel = new HydrodynamicParameter("genericLengthScaleModel", "Generic Length Scale Model", paVerticalEddy);
        HydrodynamicParameter *paMellorYamadaModel = new HydrodynamicParameter("mellorAndYamadaTurbulenceModel", "Mellor and Yamada Turbulence Model", paVerticalEddy);
        HydrodynamicParameter *paElcomMixingModel = new HydrodynamicParameter("elcomMixingModel", "ELCOM Mixing Model", paVerticalEddy);
        HydrodynamicParameter *paTurbulenceModel = new HydrodynamicParameter("turbulenceModelInBottomBoundaryLayers", "Turbulence model in the bottom boundary layers", paViscosityDiffusivity, true, false);

        parameters << paViscosityDiffusivity << paHorizontalEddy << paHorizontalEddyConstant << paSmagorinskyModel << paSmagorinskyCoefficient;
        parameters << paBackgroundHorizontalEddy << paVerticalEddy << paVerticalEddyConstant << paZeroEquationModel << paVerticalEddyViscosity;
        parameters << paBackgroundVerticalEddyViscosity << paBackgroundVerticalEddyDiffusivity << paGenericLengthScaleModel << paMellorYamadaModel;
        parameters << paElcomMixingModel << paTurbulenceModel;

        HydrodynamicParameter *paAdvectionScheme = new HydrodynamicParameter("advectionScheme", "Advection Scheme for Momentum Equation");
        HydrodynamicParameter *paEulerianMethod = new HydrodynamicParameter("eulerianLagrangianMethod", "Eulerian-Lagrangian Method", paAdvectionScheme, false);
        HydrodynamicParameter *paNumberOfSubTimeSteps = new HydrodynamicParameter("numberOfSubTimeSteps", "Number of sub-time steps", paAdvectionScheme);
        HydrodynamicParameter *paMusclMethod = new HydrodynamicParameter("secondOrderMusclMethod", "Second Order MUSCL Method", paAdvectionScheme, false);
        HydrodynamicParameter *paEnoWenoMethod = new HydrodynamicParameter("highOrderEnoWenoMethod", "High-order ENO/WENO Method", paAdvectionScheme, false);
        HydrodynamicParameter *paNoAdvectionScheme = new HydrodynamicParameter("noAdvectionScheme", "No advection scheme", paAdvectionScheme, false);

        parameters << paAdvectionScheme << paEulerianMethod << paNumberOfSubTimeSteps << paMusclMethod << paEnoWenoMethod << paNoAdvectionScheme;

        HydrodynamicParameter *paPressure = new HydrodynamicParameter("pressure", "Pressure");
        HydrodynamicParameter *paBarotropicBaroclinic = new HydrodynamicParameter("barotropicAndBaroclinicContribuitions", "Barotropic and the baroclinic contributions to the hydrostatic pressure", paPressure);
        HydrodynamicParameter *paNonHydrostaticPressure = new HydrodynamicParameter("nonHydrostaticPressure", "Non-hydrostatic pressure", paPressure, false);

        parameters << paPressure << paBarotropicBaroclinic << paNonHydrostaticPressure;
        
        HydrodynamicParameter *paCoriolis = new HydrodynamicParameter("coriolisFactor", "Coriolis factor");
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
