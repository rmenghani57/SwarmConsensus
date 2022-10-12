
"use strict";

let WaypointPull = require('./WaypointPull.js')
let CommandVtolTransition = require('./CommandVtolTransition.js')
let FileClose = require('./FileClose.js')
let CommandBool = require('./CommandBool.js')
let FileTruncate = require('./FileTruncate.js')
let VehicleInfoGet = require('./VehicleInfoGet.js')
let StreamRate = require('./StreamRate.js')
let FileList = require('./FileList.js')
let FileWrite = require('./FileWrite.js')
let LogRequestData = require('./LogRequestData.js')
let MessageInterval = require('./MessageInterval.js')
let MountConfigure = require('./MountConfigure.js')
let ParamPush = require('./ParamPush.js')
let ParamPull = require('./ParamPull.js')
let ParamSet = require('./ParamSet.js')
let CommandTOL = require('./CommandTOL.js')
let CommandTriggerInterval = require('./CommandTriggerInterval.js')
let FileRemoveDir = require('./FileRemoveDir.js')
let FileRename = require('./FileRename.js')
let FileOpen = require('./FileOpen.js')
let CommandTriggerControl = require('./CommandTriggerControl.js')
let FileMakeDir = require('./FileMakeDir.js')
let FileChecksum = require('./FileChecksum.js')
let CommandInt = require('./CommandInt.js')
let CommandHome = require('./CommandHome.js')
let FileRemove = require('./FileRemove.js')
let LogRequestEnd = require('./LogRequestEnd.js')
let SetMavFrame = require('./SetMavFrame.js')
let WaypointClear = require('./WaypointClear.js')
let WaypointSetCurrent = require('./WaypointSetCurrent.js')
let FileRead = require('./FileRead.js')
let SetMode = require('./SetMode.js')
let ParamGet = require('./ParamGet.js')
let WaypointPush = require('./WaypointPush.js')
let CommandLong = require('./CommandLong.js')
let LogRequestList = require('./LogRequestList.js')

module.exports = {
  WaypointPull: WaypointPull,
  CommandVtolTransition: CommandVtolTransition,
  FileClose: FileClose,
  CommandBool: CommandBool,
  FileTruncate: FileTruncate,
  VehicleInfoGet: VehicleInfoGet,
  StreamRate: StreamRate,
  FileList: FileList,
  FileWrite: FileWrite,
  LogRequestData: LogRequestData,
  MessageInterval: MessageInterval,
  MountConfigure: MountConfigure,
  ParamPush: ParamPush,
  ParamPull: ParamPull,
  ParamSet: ParamSet,
  CommandTOL: CommandTOL,
  CommandTriggerInterval: CommandTriggerInterval,
  FileRemoveDir: FileRemoveDir,
  FileRename: FileRename,
  FileOpen: FileOpen,
  CommandTriggerControl: CommandTriggerControl,
  FileMakeDir: FileMakeDir,
  FileChecksum: FileChecksum,
  CommandInt: CommandInt,
  CommandHome: CommandHome,
  FileRemove: FileRemove,
  LogRequestEnd: LogRequestEnd,
  SetMavFrame: SetMavFrame,
  WaypointClear: WaypointClear,
  WaypointSetCurrent: WaypointSetCurrent,
  FileRead: FileRead,
  SetMode: SetMode,
  ParamGet: ParamGet,
  WaypointPush: WaypointPush,
  CommandLong: CommandLong,
  LogRequestList: LogRequestList,
};
