package enoch.netty.session;

import enoch.netty.packet.PacketInitializer;
import io.netty.channel.Channel;

import java.util.concurrent.ConcurrentHashMap;

//TODO 접속 종료된 세션에 대한 반환 및 비활동 세션에 대한 처리 추가 필요
public class UserSessionManager {
    private ConcurrentHashMap<String, UserSession> authSessionMap;
    private ConcurrentHashMap<String, UserSession> activeSessionMap;

    private SessionPool userSessionPool;

    private static UserSessionManager instance;

    private UserSessionManager() {}

    public static UserSessionManager getInstance() {
        if(instance == null) {
            synchronized (PacketInitializer.class) {
                if(instance == null) {
                    instance = new UserSessionManager();
                    instance.init();
                }
            }
        }
        return instance;
    }

    //TODO 세션이 인증됬는지 확인용 제작 필요
    public boolean checkAuth(UserSession session) {
        if(session != null) {
            return session.getAuth();
        }
        return false;
    }

    public UserSession activeSession(Channel channel) {

        UserSession session = userSessionPool.poll();
        session.setChannel(channel);

        String sessionKey = SessionKeyGenerator.sessionKeyGen();
        session.setSessionKey(sessionKey);

        //TODO sessionKey 겹칠 경우 대책 추가 필요
        activeSessionMap.put(sessionKey, session);

        return session;
    }

    public void inactiveSession(UserSession session) {
        userSessionPool.offer(session);
    }

    public UserSession getActiveSession(String sessionKey) {
        return activeSessionMap.get(sessionKey);
    }




    private void init() {
        authSessionMap = new ConcurrentHashMap<>();
        activeSessionMap = new ConcurrentHashMap<>();
        userSessionPool = new SessionPool();
        userSessionPool.Init();
    }
}
