
"use strict";

let WaypointReached = require('./WaypointReached.js');
let RTKBaseline = require('./RTKBaseline.js');
let HilSensor = require('./HilSensor.js');
let ESCStatus = require('./ESCStatus.js');
let WheelOdomStamped = require('./WheelOdomStamped.js');
let Altitude = require('./Altitude.js');
let ParamValue = require('./ParamValue.js');
let TimesyncStatus = require('./TimesyncStatus.js');
let StatusText = require('./StatusText.js');
let CamIMUStamp = require('./CamIMUStamp.js');
let Waypoint = require('./Waypoint.js');
let ExtendedState = require('./ExtendedState.js');
let ActuatorControl = require('./ActuatorControl.js');
let WaypointList = require('./WaypointList.js');
let State = require('./State.js');
let OpticalFlowRad = require('./OpticalFlowRad.js');
let ADSBVehicle = require('./ADSBVehicle.js');
let RCIn = require('./RCIn.js');
let AttitudeTarget = require('./AttitudeTarget.js');
let OnboardComputerStatus = require('./OnboardComputerStatus.js');
let EstimatorStatus = require('./EstimatorStatus.js');
let DebugValue = require('./DebugValue.js');
let RTCM = require('./RTCM.js');
let ESCStatusItem = require('./ESCStatusItem.js');
let HilStateQuaternion = require('./HilStateQuaternion.js');
let GPSRTK = require('./GPSRTK.js');
let ESCInfoItem = require('./ESCInfoItem.js');
let Trajectory = require('./Trajectory.js');
let MountControl = require('./MountControl.js');
let HomePosition = require('./HomePosition.js');
let RCOut = require('./RCOut.js');
let RadioStatus = require('./RadioStatus.js');
let PositionTarget = require('./PositionTarget.js');
let HilGPS = require('./HilGPS.js');
let Param = require('./Param.js');
let Vibration = require('./Vibration.js');
let OverrideRCIn = require('./OverrideRCIn.js');
let GPSRAW = require('./GPSRAW.js');
let CompanionProcessStatus = require('./CompanionProcessStatus.js');
let Thrust = require('./Thrust.js');
let ESCInfo = require('./ESCInfo.js');
let LogData = require('./LogData.js');
let VFR_HUD = require('./VFR_HUD.js');
let BatteryStatus = require('./BatteryStatus.js');
let FileEntry = require('./FileEntry.js');
let HilActuatorControls = require('./HilActuatorControls.js');
let LandingTarget = require('./LandingTarget.js');
let PlayTuneV2 = require('./PlayTuneV2.js');
let HilControls = require('./HilControls.js');
let GlobalPositionTarget = require('./GlobalPositionTarget.js');
let ManualControl = require('./ManualControl.js');
let LogEntry = require('./LogEntry.js');
let Mavlink = require('./Mavlink.js');
let VehicleInfo = require('./VehicleInfo.js');
let CommandCode = require('./CommandCode.js');

module.exports = {
  WaypointReached: WaypointReached,
  RTKBaseline: RTKBaseline,
  HilSensor: HilSensor,
  ESCStatus: ESCStatus,
  WheelOdomStamped: WheelOdomStamped,
  Altitude: Altitude,
  ParamValue: ParamValue,
  TimesyncStatus: TimesyncStatus,
  StatusText: StatusText,
  CamIMUStamp: CamIMUStamp,
  Waypoint: Waypoint,
  ExtendedState: ExtendedState,
  ActuatorControl: ActuatorControl,
  WaypointList: WaypointList,
  State: State,
  OpticalFlowRad: OpticalFlowRad,
  ADSBVehicle: ADSBVehicle,
  RCIn: RCIn,
  AttitudeTarget: AttitudeTarget,
  OnboardComputerStatus: OnboardComputerStatus,
  EstimatorStatus: EstimatorStatus,
  DebugValue: DebugValue,
  RTCM: RTCM,
  ESCStatusItem: ESCStatusItem,
  HilStateQuaternion: HilStateQuaternion,
  GPSRTK: GPSRTK,
  ESCInfoItem: ESCInfoItem,
  Trajectory: Trajectory,
  MountControl: MountControl,
  HomePosition: HomePosition,
  RCOut: RCOut,
  RadioStatus: RadioStatus,
  PositionTarget: PositionTarget,
  HilGPS: HilGPS,
  Param: Param,
  Vibration: Vibration,
  OverrideRCIn: OverrideRCIn,
  GPSRAW: GPSRAW,
  CompanionProcessStatus: CompanionProcessStatus,
  Thrust: Thrust,
  ESCInfo: ESCInfo,
  LogData: LogData,
  VFR_HUD: VFR_HUD,
  BatteryStatus: BatteryStatus,
  FileEntry: FileEntry,
  HilActuatorControls: HilActuatorControls,
  LandingTarget: LandingTarget,
  PlayTuneV2: PlayTuneV2,
  HilControls: HilControls,
  GlobalPositionTarget: GlobalPositionTarget,
  ManualControl: ManualControl,
  LogEntry: LogEntry,
  Mavlink: Mavlink,
  VehicleInfo: VehicleInfo,
  CommandCode: CommandCode,
};
