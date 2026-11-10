using DAA.Producto;

namespace DAA.Implementaciones;

public static class FabricaInstancias {
    public static DatosMatriz CrearAleatoria(int n) {
        Random aleatorio = new Random();
        int[,] a = new int[n, n];
        int[,] b = new int[n, n];

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                a[i, j] = aleatorio.Next(1, 10);
                b[i, j] = aleatorio.Next(1, 10);
            }
        }
        return new DatosMatriz(a, b, n);
    }
}