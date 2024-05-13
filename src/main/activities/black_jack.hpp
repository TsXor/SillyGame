#pragma once
#ifndef __BLACK_JACK_SCENE__
#define __BLACK_JACK_SCENE__

#include <vector>
#include <unordered_set>
#include <silly_framework.hpp>

namespace acts {

namespace sf = silly_framework;

struct black_jack_state {
    enum class result_code { UNKNOWN, WIN, LOSE, TIE };
    struct round_status { ssize_t player_power, opponent_power; result_code result; };

    std::vector<size_t> player_cards, opponent_cards;
    std::unordered_set<size_t> player_selection, opponent_selection;
    std::optional<round_status> cur_round;
    size_t n_rounds, cur_round_idx, player_success, opponent_success;

    black_jack_state(size_t n_rounds_);
    ~black_jack_state();
    
    void select_card(size_t card_idx) { player_selection.insert(card_idx); }
    void unselect_card(size_t card_idx) { player_selection.erase(card_idx); }
    bool card_selected(size_t card_idx) { return player_selection.contains(card_idx); }
    
    void toggle_card(size_t card_idx);

    bool finished() { return cur_round_idx >= n_rounds; }
    bool can_submit();
    void submit_round();
    void next_round();
    result_code status();
};

class black_jack_scene : public sf::base_activity {
public:
    sf::coro_host cohost;
    std::optional<black_jack_state> state;
    size_t select_cursor = 0;

    black_jack_scene(sf::game_window& window);
    ~black_jack_scene();
    
    void render() override;
    void on_key_change() override;
    void on_key_signal() override;
    void on_tick(double this_time, double last_time) override;
};

} // namespace acts

#endif // __BLACK_JACK_SCENE__
