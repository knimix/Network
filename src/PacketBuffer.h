#pragma once
#include <queue>
#include <mutex>
#include <memory>
namespace Network{
    template<class T>
    class PacketBuffer {
    public:
        PacketBuffer() = default;
        void setLimit(int limit){mLimit = limit;}
        bool hasNext(){
            std::lock_guard guard(mMutex);
            return !mQueue.empty();
        }
        std::shared_ptr<T> next(){
            std::lock_guard guard(mMutex);
            auto tmp =  mQueue.front();
            mQueue.pop();
            return tmp;
        }
        std::shared_ptr<T>& peek(){
            std::lock_guard guard(mMutex);
            return mQueue.front();
        }
        void pop(){
            std::lock_guard guard(mMutex);
            mQueue.pop();
        }
        bool hasSpace(){
            std::lock_guard guard(mMutex);
            return mQueue.size() < mLimit;
        }
        void insert(const std::shared_ptr<T>& packet){
            std::lock_guard guard(mMutex);
            mQueue.emplace(packet);
        }
    private:
        int mLimit = 1;
        std::queue<std::shared_ptr<T>> mQueue;
        mutable std::mutex mMutex;
    };
}
