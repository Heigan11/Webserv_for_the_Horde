import java.io.*;
import java.net.MalformedURLException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.Locale;
import java.util.Map;

public class Java {

    public static String msgCreator(String msg){
        String html = "<!DOCTYPE html>\r\n" +
                "<html>\r\n" +
                "<head>\r\n" +
                "<meta charset = \"cp1251\">\r\n" +
                "<title>Ecole server</title>\r\n" +
                "<head>\r\n" +
                "<body>\r\n" +
                "<h2 align=center>" + msg + "</h2>\r\n" +
                "<body>\r\n" +
                "<html>\r\n";

        String http = "HTTP/1.1 200 OK\r\n" +
                "Connection: keep-alive\r\n" +
                "Content-type: text/html\r\n" +
                "Content-length: " + html.length() + "\r\n" +
                "\r\n" +
                html;

        return http;
    }

    public static String formCreator() {
        String html = "<!DOCTYPE html>" +
                "<html>" +
                "<head>" +
                "<meta charset = \"cp1251\">" +
                "<title>Ecole server</title>" +
                "<head>" +
                "<body>" +
                "<h2 align=center>Choose your zodiac sign</h2>" +
                "<form align=center name = \"myform\" action=\"http://localhost:8080/horoscope\" method=\"POST\">" +
                "<input name=\"AQUARIUS\" type=\"submit\" value=\"AQUARIUS\">" + "      " +
                "<input name=\"PISCES\" type=\"submit\" value=\"PISCES\">" + "      " +
                "<input name=\"ARIES\" type=\"submit\" value=\"ARIES\">" +
                "<br>" +
                "<br>" +
                "<input name=\"TAURUS\" type=\"submit\" value=\"TAURUS\">" + "      " +
                "<input name=\"GEMINI\" type=\"submit\" value=\"GEMINI\">" + "      " +
                "<input name=\"CANCER\" type=\"submit\" value=\"CANCER\">" +
                "<br>" +
                "<br>" +
                "<input name=\"LEO\" type=\"submit\" value=\"LEO\">" + "      " +
                "<input name=\"VIRGO\" type=\"submit\" value=\"VIRGO\">" + "      " +
                "<input name=\"LIBRA\" type=\"submit\" value=\"LIBRA\">" +
                "<br>" +
                "<br>" +
                "<input name=\"SCORPIO\" type=\"submit\" value=\"SCORPIO\">" + "      " +
                "<input name=\"SAGITTARIUS\" type=\"submit\" value=\"SAGITTARIUS\">" + "      " +
                "<input name=\"CAPRICORN\" type=\"submit\" value=\"CAPRICORN\">" +
                "</form>" +
                "<body>" +
                "<html>";

        String http = "HTTP/1.1 200 OK\r\n" +
                "Connection: keep-alive\r\n" +
                "Content-type: text/html\r\n" +
                "Content-length: " + html.length() + "\r\n" +
                "\r\n" +
                html;

        return http;
    }

    public static void responseToBrowser(String sign){
        if (sign == null || sign.isEmpty())
            return;
        System.out.println(msgCreator(findDescription(sign)));
    }

    public static String findDescription(String sign){
        String string = null;
        String description = null;
        int i = 0;

        try {
            URL url = new URL("https://nypost.com/horoscopes/" + sign + "/");

            try {
                LineNumberReader reader = new LineNumberReader(new InputStreamReader(url.openStream()));
                string = reader.readLine();
                while (string != null) {
                    string = reader.readLine();
                    if (string != null && string.contains("horoscope__description")){
                        ++i;
                        if (i == 2){
                        description = reader.readLine();
                        }
                    }
                }
                reader.close();
            } catch (IOException e) {
                e.printStackTrace();
            }

        } catch (MalformedURLException ex) {
            ex.printStackTrace();
        }

        description = description.substring(description.indexOf('>') + 1, description.lastIndexOf('<'));
        System.err.println(description);
        return description;
    }


    public static void main(String[] args) throws Exception {

        System.err.println("In JAVA");

        String request;
        String[] subStr;
        String method = null;

        Map<String, String> env = System.getenv();

        request = env.get("REQUEST");
        subStr = request.split(" ");
        method = subStr[0];

        assert method != null;
        if (method.equals("GET")) {
            System.out.println(formCreator());
            System.exit(0);
        }

        try {
            File file = new File("../../../../html/req_body_tmp.txt");
            FileReader fr = new FileReader(file);
            BufferedReader reader = new BufferedReader(fr);
            String line = reader.readLine();
            subStr = line.split("=");
            responseToBrowser(subStr[0].toLowerCase(Locale.ROOT));
        } catch (IOException e) {
            e.printStackTrace();
        }


//        if (env.containsKey("AQUARIUS"))
//            responseToBrowser("aquarius");
//        else if (env.containsKey("PISCES"))
//            responseToBrowser("pisces");
//        else if (env.containsKey("ARIES"))
//            responseToBrowser("aries");
//        else if (env.containsKey("TAURUS"))
//            responseToBrowser("taurus");
//        else if (env.containsKey("GEMINI"))
//            responseToBrowser("gemini");
//        else if (env.containsKey("CANCER"))
//            responseToBrowser("cancer");
//        else if (env.containsKey("LEO"))
//            responseToBrowser("leo");
//        else if (env.containsKey("VIRGO"))
//            responseToBrowser("virgo");
//        else if (env.containsKey("LIBRA"))
//            responseToBrowser("libra");
//        else if (env.containsKey("SCORPIO"))
//            responseToBrowser("scorpio");
//        else if (env.containsKey("SAGITTARIUS"))
//            responseToBrowser("sagittarius");
//        else if (env.containsKey("CAPRICORN"))
//            responseToBrowser("capricorn");
    }
}
