#include <Windows.h>
#include <ControllerDriver.h>

const float STOPPED = 0.0;
const float WALK_SPEED = 0.5;
const float RUN_SPEED = 0.95;

EVRInitError ControllerDriver::Activate(uint32_t unObjectId)
{
	driverId = unObjectId;

	PropertyContainerHandle_t props = VRProperties()->TrackedDeviceToPropertyContainer(driverId);

	VRProperties()->SetStringProperty(props, Prop_InputProfilePath_String, "{esp32}/input/controller_profile.json");
	VRProperties()->SetInt32Property(props, Prop_ControllerRoleHint_Int32, ETrackedControllerRole::TrackedControllerRole_Treadmill);
	VRDriverInput()->CreateScalarComponent(props, "/input/joystick/y", &joystickYHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
	VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/y", &trackpadYHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
	VRDriverInput()->CreateScalarComponent(props, "/input/joystick/x", &joystickXHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);
	VRDriverInput()->CreateScalarComponent(props, "/input/trackpad/x", &trackpadXHandle, EVRScalarType::VRScalarType_Absolute,
		EVRScalarUnits::VRScalarUnits_NormalizedTwoSided);

	return VRInitError_None;
}

DriverPose_t ControllerDriver::GetPose()
{
	// We don't use pose.
	DriverPose_t pose = { 0 };
	pose.poseIsValid = false;
	pose.result = TrackingResult_Calibrating_OutOfRange;
	pose.deviceIsConnected = true;

	HmdQuaternion_t quat;
	quat.w = 1;
	quat.x = 0;
	quat.y = 0;
	quat.z = 0;

	pose.qWorldFromDriverRotation = quat;
	pose.qDriverFromHeadRotation = quat;

	return pose;
}

void ControllerDriver::RunFrame()
{
	// Set movement speed.
	float currMovSpeed = WALK_SPEED;
	if (GetKeyState('R') & 0x8000) {
		currMovSpeed = RUN_SPEED;
	}

	// Direction of movement.
	if (GetKeyState('W') & 0x8000) {
		VRDriverInput()->UpdateScalarComponent(joystickYHandle, currMovSpeed, 0);
		VRDriverInput()->UpdateScalarComponent(trackpadYHandle, currMovSpeed, 0);
	}
	else if (GetKeyState('S') & 0x8000) {
		VRDriverInput()->UpdateScalarComponent(joystickYHandle, -currMovSpeed, 0);
		VRDriverInput()->UpdateScalarComponent(trackpadYHandle, -currMovSpeed, 0);
	}
	else if (!(
		(GetKeyState('W') & 0x8000) &&
		(GetKeyState('S') & 0x8000)
		)) {
		VRDriverInput()->UpdateScalarComponent(joystickYHandle, 0.0f, 0); //move forward
		VRDriverInput()->UpdateScalarComponent(trackpadYHandle, 0.0f, 0); //move foward
	}

	// We can also move sideways.
	if (GetKeyState('A') & 0x8000) {
		VRDriverInput()->UpdateScalarComponent(joystickXHandle, -currMovSpeed, 0);
		VRDriverInput()->UpdateScalarComponent(trackpadXHandle, -currMovSpeed, 0);
	}
	else if (GetKeyState('D') & 0x8000) {
		VRDriverInput()->UpdateScalarComponent(joystickXHandle, currMovSpeed, 0);
		VRDriverInput()->UpdateScalarComponent(trackpadXHandle, currMovSpeed, 0);
	}
	else if (!(
		(GetKeyState('A') & 0x8000) &&
		(GetKeyState('D') & 0x8000)
		)) {
		VRDriverInput()->UpdateScalarComponent(joystickXHandle, 0.0f, 0);
		VRDriverInput()->UpdateScalarComponent(trackpadXHandle, 0.0f, 0);
	}

}

void ControllerDriver::Deactivate()
{
	driverId = k_unTrackedDeviceIndexInvalid;
}

void* ControllerDriver::GetComponent(const char* pchComponentNameAndVersion)
{
	if (strcmp(IVRDriverInput_Version, pchComponentNameAndVersion) == 0)
	{
		return this;
	}
	return NULL;
}

void ControllerDriver::EnterStandby() {}

void ControllerDriver::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1)
	{
		pchResponseBuffer[0] = 0;
	}
}