#pragma once

#include "backend/Cue.h"

#include <vector>
#include <memory>

namespace BBackend {

    class CueList {
        private:
            std::vector<std::shared_ptr<Cue>> mCueVector{};

        public:
            CueList(){};
            ~CueList(){};

            // Data Access
            size_t size() {return mCueVector.size();}
            std::shared_ptr<Cue> operator[](size_t i) {return mCueVector[i];}

            // Data Manipulation
            void insertCues(std::vector<std::shared_ptr<Cue>>&& cues, size_t index) {
                for (auto it = cues.begin(); it < cues.end(); it++) {
                    mCueVector.insert(mCueVector.begin()+index, *it);
                }
            }
            void removeCues(std::vector<size_t> indexes) {
                for (auto it = indexes.begin(); it < indexes.end(); it++) {
                    mCueVector.erase(mCueVector.begin()+*it);
                }
            }

    };

} // namespace BBackend
