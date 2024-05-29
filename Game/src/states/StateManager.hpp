#pragma once
#include <stack>
#include "State.hpp"
#include <memory>

/**
 * @brief Manages the states.
 */
class StateManager
{
public:
	/**
	 * @brief Creates a new state manager.
	 */
	StateManager();

	/**
	 * @brief The destructor.
	 */
	~StateManager();

	/**
	 * @brief Adds a state.
	 * @param state The new state.
	 */
	void addState(std::unique_ptr<State> state);

	/**
	 * @brief Removes the current state.
	 */
	void removeState();

	/**
	 * @brief Sets the current state to a new one.
	 * @param state 
	 */
	void setState(std::unique_ptr<State> state);

	/**
	 * @brief Changes to the next state.
	 */
	void changeState();

	/**
	 * @brief Updates the current state.
	 */
	void update();

	/**
	 * @brief Draws the current state.
	 */
	void draw();

	/**
	 * @brief Gets the current state.
	 * @return The current state.
	 */
	std::unique_ptr<State> getCurrentState();

	/**
	 * @brief Is the game running.
	 * @return Whether the game is running.
	 */
	bool isRunning() const;

	/**
	 * @brief Quits the game.
	 */
	void quit();

private:
	std::stack<std::unique_ptr<State>> states;

	bool running;

	bool shouldChangeState;
	bool shouldRemoveState;
	std::unique_ptr<State> nextState;
};
