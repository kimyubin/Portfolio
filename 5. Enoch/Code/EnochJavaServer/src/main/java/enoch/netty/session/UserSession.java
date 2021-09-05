package enoch.netty.session;

import io.netty.channel.Channel;

public class UserSession {
    private String sessionKey;// 세션 고유 키(중복 x)
    private Long playerId; // 세션의 플레이어 ID(유저 고유번호)
    private Boolean isAuth = false;//접속 인증된 세션인지 체크
    private Channel channel;

    public void Init() {
        sessionKey = null;
        playerId = 0L;
        isAuth = false;
        channel = null;
    }

    //필요한 정보 추가

    public String getSessionKey() {
        return sessionKey;
    }

    public void setSessionKey(String sessionKey) {
        this.sessionKey = sessionKey;
    }

    public Long getPlayerId() {
        return playerId;
    }

    public void setPlayerId(Long playerId) {
        this.playerId = playerId;
    }

    public Boolean getAuth() {
        return isAuth;
    }

    public void setAuth(Boolean auth) {
        isAuth = auth;
    }

    public Channel getChannel() {
        return channel;
    }

    public void setChannel(Channel channel) {
        this.channel = channel;
    }
}
