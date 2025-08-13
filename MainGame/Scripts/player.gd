extends CharacterBody2D

# Constants
const JSON_PATH := "res://Others/data.json"
const JOYSTICK_CENTER := 2048.0
const SPEED := 200.0
const BUTTON_NAMES := {
	"a": "A",
	"b": "B",
	"c": "C",
	"d": "D",
	"e": "E",
	"f": "F"
}

func _physics_process(delta: float) -> void:
	handle_movement()
	handle_button_inputs()

func handle_movement() -> void:
	var velocity = Vector2.ZERO
	
	# Safely read and parse JSON data
	var data = read_joystick_data()
	if data == null:
		self.velocity = velocity
		move_and_slide()
		return

	# Process joystick input
	var input_vector = process_joystick_input(data)
	velocity = input_vector * SPEED
	
	self.velocity = velocity
	move_and_slide()

func read_joystick_data() -> Dictionary:
	if not FileAccess.file_exists(JSON_PATH):
		printerr("JSON file not found at path: ", JSON_PATH)
		return {}

	var file = FileAccess.open(JSON_PATH, FileAccess.READ)
	if file == null:
		printerr("Failed to open JSON file with error: ", FileAccess.get_open_error())
		return {}

	var content = file.get_as_text()
	file.close()
	
	var data = JSON.parse_string(content)
	if data == null or not data is Dictionary:
		printerr("Failed to parse JSON data or data is not a dictionary")
		return {}
	
	return data

func process_joystick_input(data: Dictionary) -> Vector2:
	if not data.has_all(["x", "y"]):
		printerr("Joystick data missing required keys (x, y)")
		return Vector2.ZERO

	var raw_x = float(data["x"])
	var raw_y = float(data["y"])
	
	# Normalize joystick input
	var norm_x = (raw_x - JOYSTICK_CENTER) / JOYSTICK_CENTER
	var norm_y = -(raw_y - JOYSTICK_CENTER) / JOYSTICK_CENTER  # Inverted Y axis
	
	var input_vector = Vector2(norm_x, norm_y)
	return input_vector.normalized() if input_vector.length() > 1.0 else input_vector

func handle_button_inputs() -> void:
	var file = FileAccess.open(JSON_PATH, FileAccess.READ)
	if file == null:
		return

	var content = file.get_as_text()
	file.close()
	
	var data = JSON.parse_string(content)
	if data == null or not data is Dictionary:
		return
	
	# Check each button
	for button in BUTTON_NAMES.keys():
		if data.has(button) and bool(data[button]):
			print_rich("Button %s pressed" % BUTTON_NAMES[button])
			# You could also emit signals here for other nodes to handle
			# emit_signal("button_pressed", BUTTON_NAMES[button])
