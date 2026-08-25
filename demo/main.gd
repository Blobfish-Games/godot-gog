extends HBoxContainer

@onready var gog: GOG = $GOG

var http_request: HTTPRequest
var initialized := false
var init_success: bool


func _ready() -> void:
	gog.init_finished.connect(_on_init_finished)

	var client_id: String = "CLIENT_ID"
	var client_secret: String = "CLIENT_SECRET"
	gog.initialize(client_id, client_secret)


func _on_init_finished(success: bool):
	initialized = true
	init_success = success
	prints("init finished", success)


func _on_Button_pressed():
	if not initialized:
		await gog.init_finished
	if init_success:
		print(gog.is_achievement_unlocked("ach_id"))


func _on_Button2_pressed() -> void:
	if not initialized:
		await gog.init_finished
	if init_success:
		gog.unlock_achievement("ach_id")


func _on_Button3_pressed() -> void:
	if not initialized:
		await gog.init_finished
	if init_success:
		gog.reset_stats_and_achievements()
