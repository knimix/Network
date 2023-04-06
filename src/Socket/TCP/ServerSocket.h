#include "Socket.h"
namespace Network{
    class ServerSocket {
    public:
        ServerSocket() = default;
        bool open(SocketType type);
        void close();
        inline bool isClosed() const {return mHandle == UNDEFINED_SOCKET;}
        bool bind(IPEndpoint& endpoint) const;
        void listen(int count = 2028) const;
        void accept(const std::function<void(std::shared_ptr<Socket>&)>& callback){mAcceptCallback = callback;};
        void update();
    private:
        SocketHandle mHandle = UNDEFINED_SOCKET;
        SocketType mType = SocketType::ManagedIPv4;
        PollFD mPollFD{};
        std::function<void(std::shared_ptr<Socket>&)> mAcceptCallback;
    };

}