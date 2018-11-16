#ifndef ALTOOLSET_PLAYER_HPP
#define ALTOOLSET_PLAYER_HPP

namespace altoolset {

    class Player
    {
    public:
        virtual bool init() = 0;

        virtual void play() = 0;
        virtual void stop() = 0;

        /**
         * @brief Wait unless finished
         */
        virtual void join() = 0;

        ~Player() = default;
    };

}

#endif
