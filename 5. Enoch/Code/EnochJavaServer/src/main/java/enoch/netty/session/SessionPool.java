package enoch.netty.session;

import java.util.concurrent.ConcurrentLinkedQueue;

public class SessionPool {
    //TODO 스택으로 해도되지않을까??
    ConcurrentLinkedQueue<UserSession> sessionQueue;// = new ConcurrentLinkedQueue<>();

    private final int DEFAULT_SIZE = 100;

    public void Init() {
        Init(DEFAULT_SIZE);
    }

    public void Init(int size) {
        sessionQueue = new ConcurrentLinkedQueue<UserSession>();

        for(int i = 0; i<size;i++) {
            offer(new UserSession());
        }
    }

    public void offer(UserSession session) {
        session.Init();
        if(!sessionQueue.offer(session)) {
            //TODO 실패 알림 필요??
        }
    }


    public UserSession poll(){
        UserSession session;
        if(sessionQueue.isEmpty()) {
            session = new UserSession();
            session.Init();
        } else {
            session = sessionQueue.poll();
        }

        return session;
    }


}
