# include <Siv3D.hpp>
class Trans {
public:
  Trans() = default;

  Trans(Texture rule, std::function<void()> fore_func, std::function<void()> back_func)
    : rule_shader(L"transition.hlsl"), rule_(rule),
    fore_tex_(Window::Size(), Graphics2D::GetRenderTarget().format),
    back_tex_(Window::Size(), Graphics2D::GetRenderTarget().format),
    fore_func_(fore_func), back_func_(back_func) {
  }

  void Draw(double min_vague_alpha, double max_vague_alpha) {

    ConstantBuffer<Float4> constant;
    constant->x = static_cast<float>(min_vague_alpha);
    constant->y = static_cast<float>(max_vague_alpha);

    draw(fore_tex_, constant, fore_func_);

    draw(back_tex_, constant, back_func_);

    Graphics2D::SetBlendState(BlendState::Default);
    Graphics2D::SetRenderTarget(Graphics::GetSwapChainTexture());

    Graphics2D::BeginPS(rule_shader);
    Graphics2D::SetConstant(ShaderStage::Pixel, 1, constant);
    Graphics2D::SetTexture(ShaderStage::Pixel, 1, fore_tex_);
    Graphics2D::SetTexture(ShaderStage::Pixel, 2, back_tex_);
    rule_.resize(Window::Size()).draw();
    Graphics2D::EndPS();
  }

private:

  void draw(RenderTexture& tex, const ConstantBuffer<Float4>& constant, std::function<void()> draw_func) {
    tex.clear(Color(0, 0, 0, 0));
    Graphics2D::SetRenderTarget(tex);

    // 同一画像を別のレンダーターゲットに描画しようとすると出てこなくなる
    Texture().draw();

    constexpr BlendState blend(true,
                               Blend::One, Blend::Zero, BlendOp::Add,
                               Blend::One, Blend::Zero, BlendOp::Add);
    Graphics2D::SetBlendState(blend);
    draw_func();

  }

  RenderTexture fore_tex_, back_tex_;

  std::function<void()> fore_func_, back_func_;

  Texture rule_;
  PixelShader rule_shader;

};

void Main() {
  Graphics::SetBackground(Color(60, 120, 255));

  const Texture texture(L"Example/Windmill.png");

  const Texture siv3d_kun(L"Example/siv3d-kun.png");

  const Font font(50, Typeface::Heavy);

  GUI gui(GUIStyle::Default);
  gui.setTitle(L"タイトル");
  gui.add(L"alpha", GUISlider::Create(0, 1.0, 0, true));
  gui.add(L"vague", GUISlider::Create(0, 1.0, 0, true));

  Image img(L"rule.png");
  for (auto& i : img) {
    i.a = i.g;
  }
  auto rule = Texture(img);

  Trans trans(rule, [&]() {
    siv3d_kun.draw();
  }, [&]() {
    siv3d_kun.draw(300, 0);
  });

  while (System::Update()) {
    Rect(0, 0, Window::Size())(texture).drawAt(Window::Center());

    const double mini = gui.slider(L"alpha").value;
    const double maxi = mini + gui.slider(L"vague").value;

    font(gui.slider(L"alpha").value).draw();

    trans.Draw(mini, maxi);

  }
}