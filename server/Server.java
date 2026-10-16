import com.sun.net.httpserver.HttpServer;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Map;

public class Server {
    private static final Map<String, String> TYPES = Map.of(
        "html", "text/html; charset=UTF-8",
        "css", "text/css; charset=UTF-8",
        "svg", "image/svg+xml",
        "png", "image/png",
        "jpg", "image/jpeg",
        "jpeg", "image/jpeg"
    );

    public static void main(String[] args) throws IOException {
        int port = args.length > 0 ? Integer.parseInt(args[0]) : 8080;
        Path root = Paths.get("public").toAbsolutePath().normalize();

        HttpServer server = HttpServer.create(new InetSocketAddress(port), 0);
        server.createContext("/", exchange -> {
            String uri = exchange.getRequestURI().getPath();
            Path file = root.resolve(uri.equals("/") ? "index.html" : uri.substring(1)).normalize();
            if (!file.startsWith(root) || !Files.isRegularFile(file)) {
                exchange.sendResponseHeaders(404, -1);
                exchange.close();
                return;
            }
            String name = file.getFileName().toString();
            String type = TYPES.getOrDefault(name.substring(name.lastIndexOf('.') + 1), "application/octet-stream");
            byte[] body = Files.readAllBytes(file);
            exchange.getResponseHeaders().set("Content-Type", type);
            exchange.sendResponseHeaders(200, body.length);
            try (OutputStream os = exchange.getResponseBody()) {
                os.write(body);
            }
        });
        server.start();
        System.out.println("서버 시작: http://localhost:" + port);
    }
}
