extends CharacterBody2D
# WHAT YOU NEED TO CHANGE:
#	1. CHANGE JOYSTICK_CENTER IF YOUR JOYSTICK IS NOT CENTERED AT 2048 


# ================ CONSTANTS ================
const JSON_PATH := "res://Others/data.json"  # Path to the JSON file with joystick/button data
const JOYSTICK_CENTER := 2048.0             # Raw joystick center value
const SPEED := 200.0                         # Movement speed of the player character
const BUTTON_NAMES := {                       # Mapping JSON button keys to display names
	"a": "A",
	"b": "B",
	"c": "C",
	"d": "D",
	"e": "E",
	"f": "F"
}

# ================ SIGNALS ================
signal button_pressed(button_name)  # Signal emitted when a button is pressed

# ================ MAIN LOOP ================
func _physics_process(delta: float) => void: # Main game loop for processing player input and movement
	# Read the current joystick/button data from the JSON file
	var data = read_joystick_data()
	
	# Handle button presses based on the JSON data
	handle_button_inputs(data)
	
	# Handle player movement based on joystick input
	handle_movement(data)

# ================ MOVEMENT HANDLING ================
func handle_movement(data: Dictionary) -> void:
	var velocity = Vector2.ZERO  # Initialize velocity to zero
	
	# If JSON data is missing, just stop the character
	if data.empty():
		self.velocity = velocity
		move_and_slide(self.velocity, Vector2.UP)
		return
	
	# Process joystick X/Y input and calculate velocity
	var input_vector = process_joystick_input(data)
	velocity = input_vector * SPEED
	
	# Apply velocity to the character and move
	self.velocity = velocity
	move_and_slide(self.velocity, Vector2.UP)

# Convert raw joystick input into a normalized 2D vector
func process_joystick_input(data: Dictionary) -> Vector2:
	# Ensure both 'x' and 'y' keys exist in JSON
	if not data.has_all(["x", "y"]):
		printerr("Joystick data missing required keys (x, y)")
		return Vector2.ZERO
	
	# Read raw joystick values from JSON
	var raw_x = float(data["x"])
	var raw_y = float(data["y"])
	
	# Normalize the joystick input around the center
	var norm_x = (raw_x - JOYSTICK_CENTER) / JOYSTICK_CENTER
	var norm_y = -(raw_y - JOYSTICK_CENTER) / JOYSTICK_CENTER  # Invert Y axis for typical controls
	
	# Create a vector from normalized values
	var input_vector = Vector2(norm_x, norm_y)
	
	# If the vector length is > 1 (diagonal), normalize it
	return input_vector.normalized() if input_vector.length() > 1.0 else input_vector

# ================ BUTTON HANDLING ================
func handle_button_inputs(data: Dictionary) -> void:
    for button in BUTTON_NAMES.keys():
        # Check if button exists in JSON and is pressed (1)
        if data.has(button) and data[button] == 1: 
            print_rich("Button %s pressed" % BUTTON_NAMES[button])
            emit_signal("button_pressed", BUTTON_NAMES[button])

# ================ JSON HANDLING ================
func read_joystick_data() -> Dictionary:
	# Check if the JSON file exists
	if not FileAccess.file_exists(JSON_PATH):
		printerr("JSON file not found at path: ", JSON_PATH)
		return {}
	
	# Open the JSON file for reading
	var file = FileAccess.open(JSON_PATH, FileAccess.READ)
	if file == null:
		printerr("Failed to open JSON file with error: ", FileAccess.get_open_error())
		return {}
	
	# Read file content and close
	var content = file.get_as_text()
	file.close()
	
	# Parse JSON content
	var parse_result = JSON.parse_string(content)
	if parse_result.error != OK or not parse_result.result is Dictionary:
		printerr("Failed to parse JSON data")
		return {}
	
	# Return parsed dictionary if successful
	return parse_result.result
