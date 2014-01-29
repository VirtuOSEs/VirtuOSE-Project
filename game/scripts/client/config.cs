// Torque Input Map File
if (isObject(moveMap)) moveMap.delete();
new ActionMap(moveMap);
moveMap.bindCmd(keyboard, "escape", "", "handleEscape();");
moveMap.bind(keyboard, "f2", showPlayerList);
moveMap.bind(keyboard, "ctrl h", hideHUDs);
moveMap.bind(keyboard, "alt p", doScreenShotHudless);
moveMap.bind(keyboard, "a", moveleft);
moveMap.bind(keyboard, "d", moveright);
moveMap.bind(keyboard, "left", moveleft);
moveMap.bind(keyboard, "right", moveright);
moveMap.bind(keyboard, "w", moveForward);
moveMap.bind(keyboard, "s", movebackward);
moveMap.bind(keyboard, "up", moveForward);
moveMap.bind(keyboard, "down", movebackward);
moveMap.bind(keyboard, "e", moveup);
moveMap.bind(keyboard, "c", movedown);
moveMap.bind(keyboard, "space", jump);
moveMap.bind(keyboard, "lcontrol", doCrouch);
moveMap.bind(keyboard, "lshift", doSprint);
moveMap.bind(keyboard, "f", setZoomFOV);
moveMap.bind(keyboard, "z", toggleZoom);
moveMap.bind(keyboard, "v", toggleFreeLook);
moveMap.bind(keyboard, "tab", toggleFirstPerson);
moveMap.bind(keyboard, "alt c", toggleCamera);
moveMap.bindCmd(keyboard, "ctrl k", "commandToServer(\'suicide\');", "");
moveMap.bindCmd(keyboard, "numpad1", "commandToServer(\'use\',\"Ryder\");", "");
moveMap.bindCmd(keyboard, "numpad2", "commandToServer(\'use\',\"Lurker\");", "");
moveMap.bindCmd(keyboard, "numpad3", "commandToServer(\'use\',\"LurkerGrenadeLauncher\");", "");
moveMap.bindCmd(keyboard, "numpad4", "commandToServer(\'use\',\"ProxMine\");", "");
moveMap.bindCmd(keyboard, "numpad5", "commandToServer(\'use\',\"DeployableTurret\");", "");
moveMap.bindCmd(keyboard, "r", "commandToServer(\'reloadWeapon\');", "");
moveMap.bind(keyboard, "numpad0", unmountWeapon);
moveMap.bind(keyboard, "alt w", throwWeapon);
moveMap.bind(keyboard, "alt a", tossAmmo);
moveMap.bind(keyboard, "q", nextWeapon);
moveMap.bind(keyboard, "ctrl q", prevWeapon);
moveMap.bind(keyboard, "u", toggleMessageHud);
moveMap.bind(keyboard, "pageup", pageMessageHudUp);
moveMap.bind(keyboard, "pagedown", pageMessageHudDown);
moveMap.bind(keyboard, "p", resizeMessageHud);
moveMap.bind(keyboard, "f3", startRecordingDemo);
moveMap.bind(keyboard, "f4", stopRecordingDemo);
moveMap.bind(keyboard, "f8", dropCameraAtPlayer);
moveMap.bind(keyboard, "f7", dropPlayerAtCamera);
moveMap.bindCmd(keyboard, "n", "toggleNetGraph();", "");
moveMap.bindCmd(keyboard, "ctrl z", "carjack();", "");
moveMap.bind(mouse0, "xaxis", yaw);
moveMap.bind(mouse0, "yaxis", pitch);
moveMap.bind(mouse0, "button0", changeHand);
moveMap.bind(mouse0, "button1", mouseButtonZoom);
moveMap.bind(mouse0, "zaxis", handMoveZ);
moveMap.bind(gamepad0, "thumbrx", D, "-0.23 0.23", gamepadYaw);
moveMap.bind(gamepad0, "thumbry", D, "-0.23 0.23", gamepadPitch);
moveMap.bind(gamepad0, "thumblx", D, "-0.23 0.23", gamePadMoveX);
moveMap.bind(gamepad0, "thumbly", D, "-0.23 0.23", gamePadMoveY);
moveMap.bind(gamepad0, "btn_a", jump);
moveMap.bindCmd(gamepad0, "btn_back", "disconnect();", "");
moveMap.bind(gamepad0, "btn_b", doCrouch);
moveMap.bind(gamepad0, "triggerr", gamepadFire);
moveMap.bind(gamepad0, "triggerl", gamepadAltTrigger);
moveMap.bind(gamepad0, "btn_start", toggleCamera);
moveMap.bind(gamepad0, "btn_y", toggleFirstPerson);
moveMap.bind(gamepad0, "upov", gamepadCelebrate);
moveMap.bind(gamepad0, "btn_x", gamepadReload);
moveMap.bind(gamepad0, "btn_r", nextWeapon);
moveMap.bind(gamepad0, "btn_l", prevWeapon);
moveMap.bind(oculusvr0, "ovr_sensorrotang0", OVRSensorRotEuler);
