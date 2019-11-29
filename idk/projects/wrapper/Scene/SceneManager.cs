namespace idk.SceneManagement
{
    public class SceneManager
    {
        public static void LoadScene(Scene s)
        {
            if ((object) s != null)
                Bindings.SceneChangeScene(s.guid);
        }
    }
}
