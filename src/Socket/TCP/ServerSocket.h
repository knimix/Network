#include "Socket.h"
namespace Network{
    class ServerSocket : public RawSocket{
    public:
        ServerSocket(SocketType type) : RawSocket(SocketProtocol::TCP), mType(type){};
        void listen(int count = 2048) const;
        void accept(const std::function<void(std::shared_ptr<Socket>&)>& callback){mAcceptCallback = callback;};
    private:
        void onEvent(RawSocketEvent event) override;
        SocketHandle mHandle = UNDEFINED_SOCKET;
        SocketType mType = SocketType::Managed;
        PollFD mPollFD{};
        std::function<void(std::shared_ptr<Socket>&)> mAcceptCallback;
    };

}