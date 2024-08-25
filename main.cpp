#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>

#include "Random.h"

/**
 * Represents a single standard playing card.
 */
struct Card
{
    /**
     * Represents all possible ranks a card can have.
     */
    enum Rank
    {
        ace,
        two,
        three,
        four,
        five,
        six,
        seven,
        eight,
        nine,
        ten,
        jack,
        queen,
        king,
        maxRanks
    };

    /**
     * Represents all possible suits a card can have.
     */
    enum Suit
    {
        clubs,
        diamonds,
        hearts,
        spades,
        maxSuits
    };

    Rank rank{};
    Suit suit{};

    /**
     * List of all ranks for iteration purposes.
     */
    static constexpr std::array<Rank, maxRanks> allRanks{
        ace,   two,  three, four, five,  six, seven,
        eight, nine, ten,   jack, queen, king};

    /**
     * List of all suits for iteration purposes.
     */
    static constexpr std::array<Suit, maxSuits> allSuits{clubs, diamonds,
                                                         hearts, spades};

    /**
     * List of all ranks character codes. Defined in the order they appear in
     * `Rank`.
     */
    static constexpr std::array<char, maxRanks> rankCodes{
        'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};

    /**
     * List of all suit character codes. Defined in the order they appear in
     * `Suit`.
     */
    static constexpr std::array<char, maxSuits> suitCodes{'C', 'D', 'H', 'S'};

    static_assert(allRanks.size() == maxRanks);
    static_assert(allSuits.size() == maxSuits);
    static_assert(rankCodes.size() == maxRanks);
    static_assert(suitCodes.size() == maxSuits);

    /**
     * Function overload to allowing printing of cards to `std::cout`.
     */
    friend std::ostream& operator<<(std::ostream& out, const Card& card)
    {
        out << rankCodes[card.rank] << suitCodes[card.suit];
        return out;
    }

    /**
     * Get the value of a card.
     */
    int value() const
    {
        static constexpr std::array rankValues{11, 2, 3,  4,  5,  6, 7,
                                               8,  9, 10, 10, 10, 10};
        return rankValues[rank];
    }
};

/**
 * Represents a deck of standard playing cards.
 */
class Deck
{
public:
    static inline constexpr int deckSize{52};

    /**
     * Initialise a new deck with all cards in order.
     */
    Deck()
    {
        int index{0};
        for (auto suit : Card::allSuits)
        {
            for (auto rank : Card::allRanks)
            {
                m_cards[index] = Card{rank, suit};
                ++index;
            }
        }
    }

    /**
     * Deal the next card in the deck.
     *
     * @returns the next card in the deck.
     */
    Card dealCard()
    {
        assert(m_nextCard < 52 && "Deck is empty!");
        return m_cards[m_nextCard++];
    }

    /**
     * Shuffle all cards and reset deal card counter back to the start of the
     * deck.
     */
    void shuffle()
    {
        std::shuffle(m_cards.begin(), m_cards.end(), Random::mt);
        m_nextCard = 0;
    }

private:
    std::array<Card, deckSize> m_cards{};
    int m_nextCard{0};
};

/**
 * Represents a player.
 */
struct Player
{
    int score{};
};

namespace Settings
{
static constexpr int bustScore{21};
static constexpr int dealerScoreLimit{17};

}  // namespace Settings

/**
 * Get a player's next action.
 *
 * @returns true if player hits. false if player stands.
 */
bool getPlayerAction()
{
    while (true)
    {
        char action{};
        std::cout << "(h) to hit, or (s) to stand: ";
        std::cin >> action;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (std::cin) switch (action)
            {
            case 'h':
                return true;
            case 's':
                return false;
            default:
                std::cout
                    << "Input was not a valid action! Please try again.\n";
                continue;
            }
        else
        {
            std::cin.clear();
            std::cout << "Input was not recognized! Please try again.\n";
        }
    }
}

/**
 * Perform's the player's turn.
 *
 * @returns true if the player goes bust.
 */
bool playerTurn(Player& player, Deck& deck)
{
    while (player.score <= Settings::bustScore)
    {
        bool playerChoseHit = getPlayerAction();

        if (playerChoseHit)
        {
            Card card{deck.dealCard()};
            player.score += card.value();
            std::cout << "You were dealt " << card
                      << ". You now have: " << player.score << '\n';
        }
        else
            return false;
    }
    std::cout << "You went bust!\n";
    return true;
}

/**
 * Performs the dealer's turn.
 *
 * @returns boolean. true if the dealer went bust.
 */
bool dealerTurn(Player& dealer, Deck& deck)
{
    while (dealer.score <= Settings::dealerScoreLimit)
    {
        Card card{deck.dealCard()};
        dealer.score += card.value();
        std::cout << "The dealer flips a " << card
                  << ". They now have: " << dealer.score << '\n';

        if (dealer.score > Settings::bustScore)
        {
            std::cout << "The dealer went bust!\n";
            return true;
        }
    }
    return false;
}

bool blackjack()
{
    Deck deck{};
    deck.shuffle();

    Player player{};
    Player dealer{};

    player.score = deck.dealCard().value();
    dealer.score = deck.dealCard().value();

    std::cout << "The dealer is showing: " << dealer.score << '\n';
    std::cout << "You have score: " << player.score << '\n';

    if (playerTurn(player, deck)) return false;
    if (dealerTurn(dealer, deck)) return true;

    if (dealer.score < player.score)
        return true;
    else
        return false;
}

int main()
{
    bool playerWon = blackjack();
    if (playerWon)
        std::cout << "You win!\n";
    else
        std::cout << "You lose!\n";
}
