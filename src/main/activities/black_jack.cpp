#include <algorithm>
#include <random>
#include "./black_jack.hpp"

using namespace acts;
using namespace silly_framework;

static std::random_device randev;

static const size_t card_per_round = 3;
static const size_t power_limit = 21;
static const size_t max_card_id = 52;
static inline size_t card_power(size_t card_id) { return (card_id - 1) % 13 + 1; }
static inline size_t card_type(size_t card_id) { return (card_id - 1) / 13; }
static inline size_t cut_power(size_t power) { return (power > power_limit) ? power_limit - power : power; }

static inline std::vector<size_t> shuffle_iota(size_t start, size_t stop) {
    std::vector<size_t> result(stop - start, 0);
    for (size_t i = 0; i < result.size(); ++i) { result[i] = i + start; }
    std::mt19937 rangen(randev());
    std::shuffle(result.begin(), result.end(), rangen);
    return result;
}

black_jack_state::black_jack_state(size_t n_rounds_):
n_rounds(n_rounds_), cur_round_idx(0), player_success(0), opponent_success(0),
player_cards(n_rounds_ * card_per_round, 0), opponent_cards(n_rounds_ * card_per_round, 0) {
    auto card_arr_size = n_rounds_ * card_per_round;
    auto shuffled = shuffle_iota(1, max_card_id + 1);
    auto copy_pos = shuffled.begin();
    std::copy_n(copy_pos, card_arr_size, player_cards.begin());
    copy_pos += card_arr_size;
    std::copy_n(copy_pos, card_arr_size, opponent_cards.begin());
}

black_jack_state::~black_jack_state() {}

void black_jack_state::toggle_card(size_t card_idx) {
    if (card_selected(card_idx)) {
        unselect_card(card_idx);
    } else if (player_selection.size() < card_per_round) {
        select_card(card_idx);
    }
}

template <typename T>
static inline auto erase_idxs(std::vector<T>& vec, const std::unordered_set<size_t>& idxs) {
    std::vector<T> result; result.reserve(vec.size() - idxs.size());
    for (size_t i = 0; i < vec.size(); ++i) {
        if (!idxs.contains(i)) { result.push_back(std::move(vec[i])); }
    }
    vec.swap(result);
}

bool black_jack_state::can_submit() {
    return player_selection.size() == card_per_round;
}

void black_jack_state::submit_round() {
    if (finished()) { return; }

    auto choice_vec = shuffle_iota(0, opponent_cards.size());
    for (size_t i = 0; i < card_per_round; ++i) { opponent_selection.insert(choice_vec[i]); }

    cur_round.emplace();

    cur_round->player_power = 0;
    for (auto&& idx : player_selection) { cur_round->player_power += card_power(player_cards[idx]); }
    cur_round->player_power = cut_power(cur_round->player_power);

    cur_round->opponent_power = 0;
    for (auto&& idx : opponent_selection) { cur_round->opponent_power += card_power(opponent_cards[idx]); }
    cur_round->opponent_power = cut_power(cur_round->opponent_power);

    if (cur_round->player_power == cur_round->opponent_power) {
        if (cur_round->player_power == power_limit) {
            ++player_success; ++opponent_success; 
        }
        cur_round->result = result_code::TIE;
    } else if (cur_round->player_power > cur_round->opponent_power) {
        ++player_success; cur_round->result = result_code::WIN;
    } else if (cur_round->player_power < cur_round->opponent_power) {
        ++opponent_success; cur_round->result = result_code::LOSE;
    }
}

void black_jack_state::next_round() {
    erase_idxs(player_cards, player_selection); player_selection.clear();
    erase_idxs(opponent_cards, opponent_selection); opponent_selection.clear();
    cur_round.reset();
    ++cur_round_idx;
}

black_jack_state::result_code black_jack_state::status() {
    if (!finished()) { return result_code::UNKNOWN; }
    if (player_success == opponent_success) {
        return result_code::TIE;
    } else if (player_success > opponent_success) {
        return result_code::WIN;
    } else if (player_success < opponent_success) {
        return result_code::LOSE;
    } else {
        return result_code::UNKNOWN; // 不可能发生
    }
}

black_jack_scene::black_jack_scene(game_window &window) : base_activity(window) {
    parent.renman.vs_size(1024, 768);
    
    coutils::sync::unleash_lambda([&]() -> coutils::async_fn<void> {
        
        auto wait_key = [&](vkey::code key) -> coutils::async_fn<void> {
            while (true) {
                co_await cohost.wait_event({cohost.EVT_KEY_CHANGE});
                if (parent.inpman.pressed(key)) { break; }
            }
        };

        co_await wait_key(vkey::code::CHECK);
        state.emplace(3); // 初始化一轮三局21点
        while (true) {
            while (true) {
                co_await cohost.wait_event({cohost.EVT_KEY_CHANGE});
                if (parent.inpman.pressed(vkey::code::CHECK)) {
                    if (select_cursor == state->player_cards.size()) {
                        if (state->can_submit()) {
                            state->submit_round(); select_cursor = 0; break;
                        }
                    } else {
                        state->toggle_card(select_cursor);
                    }
                } else if (parent.inpman.pressed(vkey::code::LEFT)) {
                    if (select_cursor != 0) { --select_cursor; }
                } else if (parent.inpman.pressed(vkey::code::RIGHT)) {
                    if (select_cursor < state->player_cards.size()) { ++select_cursor; }
                }
            }
            co_await wait_key(vkey::code::CHECK);
            state->next_round();
            if (state->finished()) {
                co_await wait_key(vkey::code::CHECK);
                state.reset(); break;
            }
        }
        finish();
    });
}

black_jack_scene::~black_jack_scene() {}

void black_jack_scene::render_img(const std::string& path, const glut::position& pos) {
    auto&& [vs_w, vs_h] = parent.renman.vs_size();
    auto img = parent.texman.get_texture(path);
    if (img != nullptr) { parent.renman.blit(img->tex, glut::xy_trans(pos, vs_w, vs_h, glut::eye4), glut::full_uv); }
}

void black_jack_scene::render_bg(const std::string& path) {
    auto img = parent.texman.get_texture(path);
    if (img != nullptr) { parent.renman.blit(img->tex, glut::full_xy, glut::full_uv); }
}

void black_jack_scene::render() {
    render_bg("black_jack_table.png");
    if (state) {
        auto render_poker = [&](size_t card_id, const glut::position& pos) {
            render_img(std::format("pokers/{}.png", card_id), pos);
        };
        auto render_number = [&](ssize_t n, const glut::position& pos) {
            auto ns = std::to_string(n);
            auto rpos = pos;
            for (auto&& nc : ns) {
                render_img(std::format("power_numbers/{}.png", nc), rpos);
                rpos += glut::coord(pos.width(), 0);
            }
        };
        
        glut::position my_poker_start_pos = {80, 212, 540, 722};
        for (size_t i = 0; i < state->player_cards.size(); ++i) {
            auto poker_pos = my_poker_start_pos + glut::coord(80 * i, 0);
            if (!state->cur_round) {
                if (i == select_cursor) { poker_pos += glut::coord(0, -40); }
                if (state->player_selection.contains(i)) { poker_pos += glut::coord(0, -40); }
            } else {
                if (state->player_selection.contains(i)) { poker_pos += glut::coord(0, -80); }
            }
            render_poker(state->player_cards[i], poker_pos);
        }
        bool btn_selected = select_cursor == state->player_cards.size();
        render_img(btn_selected ? "go_button_focus.png" : "go_button_idle.png", {886, 992, 598, 662});

        render_number(state->opponent_success, {640, 688, 64, 160});
        render_number(state->player_success, {900, 948, 480, 576});

        if (state->cur_round) {
            glut::position his_poker_start_pos = {336, 468, 240, 422};
            for (size_t i = 0; auto&& idx : state->opponent_selection) {
                auto poker_pos = his_poker_start_pos + glut::coord(160 * i, 0);
                render_poker(state->opponent_cards[idx], poker_pos); ++i;
            }

            render_number(state->cur_round->opponent_power, {64, 88, 256, 304});
            render_number(state->cur_round->player_power, {64, 88, 360, 408});
        }

        if (state->finished()) {
            using enum black_jack_state::result_code;
            auto result = state->status();
            if (result == LOSE || result == TIE) { render_img("trophy.png", {288, 416, 32, 160}); }
            if (result == WIN || result == TIE) { render_img("trophy.png", {32, 160, 512, 640}); }
        }
    }
}

void black_jack_scene::on_key_change() {
    cohost.on_key_change();
}

void black_jack_scene::on_key_signal() {
    cohost.on_key_signal();
}

void black_jack_scene::on_tick(double this_time, double last_time) {
    cohost.on_tick(this_time, last_time);
}
