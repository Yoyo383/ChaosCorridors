#pragma once

class StateManager;

/**
 * @brief State class (virtual).
 */
class State
{
public:
	/**
	 * @brief Updates the state.
	 */
	virtual void update() = 0;

	/**
	 * @brief Draws the state.
	 */
	virtual void draw() = 0;
};

