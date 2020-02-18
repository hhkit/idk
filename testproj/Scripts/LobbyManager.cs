using idk;
using System.Collections;

namespace TestAndSeek
{
    public class LobbyManager
        : MonoBehavior
    {
        public GameObject player0; // 0 is host player
        public GameObject player1;
        public GameObject player2;
        public GameObject player3;

        void Start()
        {
            /*
            ElectronNetwork.OnClientConnected += 
                (Player p) =>
                {
                    Debug.Log("Player " + p.ActorNumber + " connected!");
                    switch (p.ActorNumber)
                    {
                        case 0: player1.GetComponent<ElectronView>().TransferOwnership(p); break;
                        case 1: player2.GetComponent<ElectronView>().TransferOwnership(p); break;
                        case 2: player3.GetComponent<ElectronView>().TransferOwnership(p); break;
                    }
                };
            ElectronNetwork.OnClientDisconnected += 
                (Player p) => 
                { 
                    Debug.Log("Player " + p.ActorNumber + " disconnected!");
                    switch (p.ActorNumber)
                    {
                        case 0: player1.GetComponent<ElectronView>().TransferOwnership(null); break;
                        case 1: player2.GetComponent<ElectronView>().TransferOwnership(null); break;
                        case 2: player3.GetComponent<ElectronView>().TransferOwnership(null); break;
                    }
                };

            ElectronNetwork.OnServerConnected += () => { Debug.Log("connected to server!"); };
            ElectronNetwork.OnServerDisconnected += () => { Debug.Log("disconnected from server!"); };
            */
            StartCoroutine(TransferObjects());

        }

        IEnumerator TransferObjects()
        {
            yield return null;
            yield return null;
            foreach (var p in ElectronNetwork.GetPlayers())
            {
                PlayerController c = null;
                switch (p.ActorNumber)
                {
                    case 0: c = player1.GetComponent<PlayerController>(); break;
                    case 1: c = player2.GetComponent<PlayerController>(); break;
                    case 2: c = player3.GetComponent<PlayerController>(); break;
                }

                if (c != null)
                {
                    c.p = p;
                    c.transfer = true;
                }
            }

        }

        void Update()
        {
        }
    }
}
