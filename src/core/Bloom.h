/**
 * @file src/core/Bloom.h
 * @author Matheus de Camargo Marques <matheuscamarques@gmail.com>
 * @brief Efeito de brilho por captura com bright-pass, blur separável e soma aditiva.
 * @details Fotografa a cena em endScene, extrai brilho, borra e soma antes do HUD, usado por Game, vira no-op sem shader disponível.
 */

#pragma once
#include <SFML/Graphics.hpp>
#include <cstdio>

namespace core {

// Bloom por captura (item 21): fotografa o framebuffer da cena (sem HUD),
// extrai bright-pass, borra separável 9-tap e soma de volta aditivo.
// Sem reestruturação do render: a cena continua desenhada no window;
// endScene() captura, processa e soma o brilho antes do HUD.
// Sem GL/disponibilidade de shader: vira no-op (cena intacta).
class Bloom {
public:
    // GL precisa de contexto: chamar no boot com a janela aberta.
    void init(unsigned w, unsigned h) {
        if (w == 0 || h == 0) return;
        sceneTex_.create(w, h);
        bright_.create(w, h);
        blurA_.create(w, h);
        blurB_.create(w, h);
        // Log por etapa (30s de debug no console): sem isso o no-op é
        // silencioso e o bloom "não roda" sem deixar rastro.
        if (!sf::Shader::isAvailable()) {
            std::fprintf(stderr, "[bloom] off: shaders indisponíveis\n");
            return;
        }
        if (!brightPass_.loadFromMemory(kBrightPass, sf::Shader::Fragment)) {
            std::fprintf(stderr, "[bloom] off: bright-pass não compilou\n");
            return;
        }
        if (!blur_.loadFromMemory(kBlur, sf::Shader::Fragment)) {
            std::fprintf(stderr, "[bloom] off: blur não compilou\n");
            return;
        }
        ok_ = true;
    }

    // Captura a cena do window, soma o bloom aditivo. HUD vem depois.
    void endScene(sf::RenderWindow& window, float threshold = 0.6f,
                  float intensity = 0.6f) {
        if (!ok_) return;
        sceneTex_.update(window);

        bright_.clear(sf::Color::Black);
        sf::Sprite sceneSprite(sceneTex_);
        brightPass_.setUniform("texture", sceneTex_);
        brightPass_.setUniform("threshold", threshold);
        brightPass_.setUniform("softKnee", 0.5f);
        bright_.draw(sceneSprite, &brightPass_);
        bright_.display();

        const sf::Vector2u sz = bright_.getSize();
        const sf::Glsl::Vec2 texel(1.f / sz.x, 1.f / sz.y);
        blurA_.clear(sf::Color::Black);
        blur_.setUniform("texture", bright_.getTexture());
        blur_.setUniform("direction", sf::Glsl::Vec2(1.f, 0.f));
        blur_.setUniform("texelSize", texel);
        sf::Sprite brightSprite(bright_.getTexture());
        blurA_.draw(brightSprite, &blur_);
        blurA_.display();

        blurB_.clear(sf::Color::Black);
        blur_.setUniform("texture", blurA_.getTexture());
        blur_.setUniform("direction", sf::Glsl::Vec2(0.f, 1.f));
        blur_.setUniform("texelSize", texel);
        sf::Sprite blurASprite(blurA_.getTexture());
        blurB_.draw(blurASprite, &blur_);
        blurB_.display();

        sf::Sprite bloomSprite(blurB_.getTexture());
        bloomSprite.setColor(sf::Color(
            255, 255, 255, static_cast<sf::Uint8>(255.f * intensity)));
        window.draw(bloomSprite, sf::BlendAdd);
    }

private:
    // SFML 2.x: varyings legados (gl_TexCoord) — válidos nesta versão.
    static constexpr const char* kBrightPass = R"(
        uniform sampler2D texture;
        uniform float threshold;
        uniform float softKnee;
        void main() {
            vec4 c = texture2D(texture, gl_TexCoord[0].xy);
            float brightness = dot(c.rgb, vec3(0.2126, 0.7152, 0.0722));
            float knee = threshold * softKnee + 1e-5;
            float soft = clamp(brightness - threshold + knee, 0.0, 2.0 * knee);
            soft = soft * soft / (4.0 * knee + 1e-5);
            float contrib = max(soft, brightness - threshold) / max(brightness, 1e-5);
            gl_FragColor = vec4(c.rgb * contrib, 1.0);
        }
    )";
    static constexpr const char* kBlur = R"(
        uniform sampler2D texture;
        uniform vec2 direction;
        uniform vec2 texelSize;
        void main() {
            vec2 off1 = vec2(1.3846153846) * direction;
            vec2 off2 = vec2(3.2307692308) * direction;
            vec4 c = texture2D(texture, gl_TexCoord[0].xy) * 0.2270270270;
            c += texture2D(texture, gl_TexCoord[0].xy + off1 * texelSize) * 0.3162162162;
            c += texture2D(texture, gl_TexCoord[0].xy - off1 * texelSize) * 0.3162162162;
            c += texture2D(texture, gl_TexCoord[0].xy + off2 * texelSize) * 0.0702702703;
            c += texture2D(texture, gl_TexCoord[0].xy - off2 * texelSize) * 0.0702702703;
            gl_FragColor = c;
        }
    )";

    sf::Texture sceneTex_;
    sf::RenderTexture bright_, blurA_, blurB_;
    sf::Shader brightPass_, blur_;
    bool ok_ = false;
};

} // namespace core
