#include "ColorTextureProgram.hpp"

#include "Mode.hpp"
#include "GL.hpp"

#include <glm/glm.hpp>

#include <random>
#include <vector>
#include <deque>

/*
 * MagicPongMode is a game mode that implements a single-player game of Pong.
 */

struct MagicPongMode : Mode {
	MagicPongMode();
	virtual ~MagicPongMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	glm::vec2 court_radius = glm::vec2(7.0f, 5.0f);
	glm::vec2 paddle_radius = glm::vec2(0.2f, 1.0f);
	glm::vec2 ball_radius = glm::vec2(0.2f, 0.2f);

	glm::vec2 left_paddle = glm::vec2(-court_radius.x + 0.5f, 0.0f);
	glm::vec2 right_paddle = glm::vec2( court_radius.x - 0.5f, 0.0f);

	glm::vec2 ball = glm::vec2(0.0f, 0.0f);
	glm::vec2 ball_velocity = glm::vec2(-1.0f, 0.0f);

	uint32_t left_score = 0;
	uint32_t right_score = 0;

	float ai_offset = 0.0f;
	float ai_offset_update = 0.0f;

	enum class Item : uint8_t {
		NONE = 0,
		LONGER_PADDLE,
		SHORTER_PADDLE,
		FASTER_BALL
	};

	Item current_item = Item::NONE;
	glm::vec2 item_pos = glm::vec2(0.0f, 0.0f);

	
	static constexpr glm::vec2 item_radius = glm::vec2(0.2f, 0.2f);
	static constexpr glm::vec2 item_generation_center = glm::vec2(0.0f, 0.0f);
	static constexpr glm::vec2 item_generation_radius = glm::vec2(5.0f, 3.0f);

	float left_paddle_effect_time = 0.0f;
	float right_paddle_effect_time = 0.0f;
	glm::vec2 paddle_radius_modifier = glm::vec2(0.0f, 0.0f);

	float accelerate_ball_effect_time = 0.0f;

	static constexpr float kBallAccelerationModifier = 1.5f;

	glm::vec2 ball_vertical_velocity_modifier = glm::vec2(0.0f, 0.0f);

	bool player_turn = false;
	//----- pretty rainbow trails -----

	float trail_length = 1.3f;
	std::deque< glm::vec3 > ball_trail; //stores (x,y,age), oldest elements first

	//----- opengl assets / helpers ------

	//draw functions will work on vectors of vertices, defined as follows:
	struct Vertex {
		Vertex(glm::vec3 const &Position_, glm::u8vec4 const &Color_, glm::vec2 const &TexCoord_) :
			Position(Position_), Color(Color_), TexCoord(TexCoord_) { }
		glm::vec3 Position;
		glm::u8vec4 Color;
		glm::vec2 TexCoord;
	};
	static_assert(sizeof(Vertex) == 4*3 + 1*4 + 4*2, "MagicPongMode::Vertex should be packed");

	//Shader program that draws transformed, vertices tinted with vertex colors:
	ColorTextureProgram color_texture_program;

	//Buffer used to hold vertex data during drawing:
	GLuint vertex_buffer = 0;

	//Vertex Array Object that maps buffer locations to color_texture_program attribute locations:
	GLuint vertex_buffer_for_color_texture_program = 0;

	//Solid white texture:
	GLuint white_tex = 0;

	//matrix that maps from clip coordinates to court-space coordinates:
	glm::mat3x2 clip_to_court = glm::mat3x2(1.0f);
	// computed in draw() as the inverse of OBJECT_TO_CLIP
	// (stored here so that the mouse handling code can use it to position the paddle)

	std::mt19937 mt; //mersenne twister pseudo-random number generator


private:
	void GenerateNewItem();
};
