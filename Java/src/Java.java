import java.io.*;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Locale;
import java.util.Map;

public class Java {

    public static String msgCreator(String msg){
        String html = "<!DOCTYPE html>\r\n" +
                "<html>\r\n" +
                "<head>\r\n" +
                "<meta charset = \"cp1251\">\r\n" +
                "<title>Ecole server</title>\r\n" +
                "<style> body {background: #000 url(https://cdn.pixabay.com/photo/2016/11/23/17/13/dark-1853877_960_720.jpg); color: #fff; background-attachment: fixed; background-repeat: no-repeat; background-size: cover;} </style>\r\n" +
                "<head>\r\n" +
                "<body>" +
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
                "<button type=\"submit\" name=\"AQUARIUS\" value=\"AQUARIUS\"><img src=\"https://cdn.pixabay.com/photo/2015/05/09/07/32/aquarius-759383__340.jpg\" border=\"0\" alt=\"SomeAlternateText\" style=\"width: 150px;\"></button>" + "       " +
                "<button type=\"submit\" name=\"PISCES\" value=\"PISCES\"><img src=\"https://cdn.pixabay.com/photo/2015/05/09/07/32/fish-759384__340.jpg\" border=\"0\" alt=\"SomeAlternateText\" style=\"width: 150px;\"></button>" + "       " +
                "<button type=\"submit\" name=\"ARIES\" value=\"ARIES\"><img src=\"https://cdn.pixabay.com/photo/2015/05/09/07/32/aries-759382__340.jpg\" border=\"0\" alt=\"SomeAlternateText\" style=\"width: 150px;\"></button>" + "       " +
                "<br>" +
                "<br>" +
                "<button type=\"submit\" name=\"TAURUS\" value=\"TAURUS\"><img src=\"https://cdn.pixabay.com/photo/2015/05/09/07/32/bull-759381__340.jpg\" border=\"0\" alt=\"SomeAlternateText\" style=\"width: 150px;\"></button>" + "       " +
                "<button type=\"submit\" name=\"GEMINI\" value=\"GEMINI\"><img src=\"https://cdn.pixabay.com/photo/2015/05/09/07/30/twins-759375__340.jpg\" border=\"0\" alt=\"SomeAlternateText\" style=\"width: 150px;\"></button>" + "       " +
                "<button type=\"submit\" name=\"CANCER\" value=\"CANCER\"><img src=\"https://cdn.pixabay.com/photo/2015/05/09/07/31/cancer-759378__340.jpg\" border=\"0\" alt=\"SomeAlternateText\" style=\"width: 150px;\"></button>" + "       " +
                "<br>" +
                "<br>" +
                "<button type=\"submit\" name=\"LEO\" value=\"LEO\"><img src=\"https://cdn.pixabay.com/photo/2015/05/09/07/30/lion-759374__340.jpg\" border=\"0\" alt=\"SomeAlternateText\" style=\"width: 150px;\"></button>" + "       " +
                "<button type=\"submit\" name=\"VIRGO\" value=\"VIRGO\"><img src=\"https://cdn.pixabay.com/photo/2015/05/09/07/31/virgin-759376__340.jpg\" border=\"0\" alt=\"SomeAlternateText\" style=\"width: 150px;\"></button>" + "       " +
                "<button type=\"submit\" name=\"LIBRA\" value=\"LIBRA\"><img src=\"https://cdn.pixabay.com/photo/2015/05/09/07/31/horizontal-759380__340.jpg\" border=\"0\" alt=\"SomeAlternateText\" style=\"width: 150px;\"></button>" + "       " +
                "<br>" +
                "<br>" +
                "<button type=\"submit\" name=\"SCORPIO\" value=\"SCORPIO\"><img src=\"https://cdn.pixabay.com/photo/2015/05/09/07/31/scorpio-759377__340.jpg\" border=\"0\" alt=\"SomeAlternateText\" style=\"width: 150px;\"></button>" + "       " +
                "<button type=\"submit\" name=\"SAGITTARIUS\" value=\"SAGITTARIUS\"><img src=\"https://cdn.pixabay.com/photo/2015/05/09/07/30/contactors-759373__340.jpg\" border=\"0\" alt=\"SomeAlternateText\" style=\"width: 150px;\"></button>" + "       " +
                "<button type=\"submit\" name=\"CAPRICORN\" value=\"CAPRICORN\"><img src=\"https://cdn.pixabay.com/photo/2015/05/09/07/31/capricorn-759379__340.jpg\" border=\"0\" alt=\"SomeAlternateText\" style=\"width: 150px;\"></button>" + "       " +
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
    }
}
