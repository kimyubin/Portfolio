package enoch.netty.session;

import java.util.Random;

public class SessionKeyGenerator {

    private static final String genString = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    public static String sessionKeyGen() {
        StringBuffer temp = new StringBuffer();
        Random rnd = new Random();
        for (int i = 0; i < 16; i++) {
            temp.append(rnd.nextInt(genString.length()));
        }
        return temp.toString();
    }
}
