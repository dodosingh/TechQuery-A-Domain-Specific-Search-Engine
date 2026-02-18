import 'package:flutter/material.dart';
import 'package:url_launcher/url_launcher.dart';

class SearchResultsPage extends StatelessWidget {
  final String query;

  const SearchResultsPage({super.key, required this.query});

  Future<void> _launchURL(String url) async {
    final Uri uri = Uri.parse(url);
    if (!await launchUrl(uri, mode: LaunchMode.externalApplication)) {
      throw 'Could not launch $url';
    }
  }

  @override
  Widget build(BuildContext context) {
    // Dummy results
    final List<Map<String, String>> dummyResults = [
      {
        "title": "Apple's Latest M3 Chip Revolutionizes Performance",
        "description": "Discover how Apple’s M3 chip is setting new standards in efficiency and AI-driven computing.",
        "url": "https://www.techcrunch.com/apple-m3-chip"
      },
      {
        "title": "GSM Arena Reviews the Samsung Galaxy S25 Ultra",
        "description": "A complete breakdown of Samsung’s newest flagship — specs, camera, and more.",
        "url": "https://www.gsmarena.com/samsung_galaxy_s25_ultra_review"
      },
      {
        "title": "AI in Everyday Devices: The Next Big Leap",
        "description": "TechRadar explores how AI integration is changing the way we interact with gadgets.",
        "url": "https://www.techradar.com/ai-devices-future"
      },
      {
        "title": "Tesla’s New Cybertruck — Built for the Future",
        "description": "Elon Musk unveils the redesigned Cybertruck — now with smarter sensors and cleaner energy use.",
        "url": "https://www.theverge.com/tesla-cybertruck-2025"
      },
    ];

    return Scaffold(
      backgroundColor: const Color(0xFF0D0D0D),
      appBar: AppBar(
        backgroundColor: Colors.transparent,
        elevation: 0,
        title: Text(
          "Results for \"$query\"",
          style: const TextStyle(
            color: Colors.white,
            fontWeight: FontWeight.bold,
            letterSpacing: 1.2,
          ),
        ),
        centerTitle: true,
      ),
      body: Padding(
        padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 10),
        child: ListView.builder(
          itemCount: dummyResults.length,
          itemBuilder: (context, index) {
            final result = dummyResults[index];
            return GestureDetector(
              onTap: () => _launchURL(result["url"]!),
              child: Container(
                margin: const EdgeInsets.symmetric(vertical: 10),
                padding: const EdgeInsets.all(20),
                decoration: BoxDecoration(
                  color: const Color(0xFF1A1A1A),
                  borderRadius: BorderRadius.circular(20),
                  boxShadow: [
                    BoxShadow(
                      color: Colors.deepPurpleAccent.withOpacity(0.2),
                      blurRadius: 10,
                      offset: const Offset(0, 4),
                    ),
                  ],
                ),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      result["title"]!,
                      style: const TextStyle(
                        color: Colors.white,
                        fontSize: 18,
                        fontWeight: FontWeight.bold,
                        letterSpacing: 1.1,
                      ),
                    ),
                    const SizedBox(height: 8),
                    Text(
                      result["description"]!,
                      style: const TextStyle(
                        color: Colors.grey,
                        fontSize: 14,
                        height: 1.5,
                      ),
                    ),
                    const SizedBox(height: 10),
                    Text(
                      result["url"]!,
                      style: const TextStyle(
                        color: Colors.deepPurpleAccent,
                        fontSize: 13,
                        decoration: TextDecoration.underline,
                      ),
                    ),
                  ],
                ),
              ),
            );
          },
        ),
      ),
    );
  }
}
